/*
TCP Echo server example in winsock
Live Server on port 10000
*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#pragma warning( disable : 4996 34 )  

#define SWAP(a, b){int t; t=a; a=b; b=t;}

void shuffle(int *a) {
	int pos;
	for (int i = 1; i < 53; i++) { //故意不從0開始，以免空牌與第一張牌都是零
		a[i-1] = i;
	}
	srand((unsigned)time(NULL));
	rand();
	for (int i = 0; i < 52; i++) {
		pos = (int)52 * (float)rand() / RAND_MAX;
		SWAP(a[i], a[pos]);
	}
}
void print_card(int card) {
	int card_suit = (card-1) / 13;
	int card_num = card % 13;
	switch (card_num ) {
	case 11:
		printf("%c%c  ", card_suit + 3, 'J');
		break;
	case 12:
		printf("%c%c  ", card_suit + 3, 'Q');
		break;
	case 0:
		printf("%c%c  ", card_suit + 3, 'K');
		break;
	case 1:
		printf("%c%c  ", card_suit + 3, 'A');
		break;
	default:
		printf("%c%d  ", card_suit + 3, card_num );
		break;
	}
}
void print_all_card(int a[][5],int num) {
	for (int i = 0; i < 5; i++) {
		if (a[num][i] != 0) {
			print_card(a[num][i]);
		}
	}
}
int score_calculation(int a[][5],int num) {
	int score = 0, temp=0;
	for (int i = 0; i < 5; i++) {
		temp = a[num][i] % 13 ;
		if (temp == 0 && a[num][i] > 0) { temp = 13; }
		if (temp > 10 ) { temp = 10; }
		score += temp;
	}
	return score;
}
int output_game_result(int *scores,int max_clients) {
	int max_points = 0;
	int max_player = 0; //記錄贏家編號
	int game_result;
	//scores[0]包含莊家，所以i < ( max_clients+1)
	for (int i = 0; i < (max_clients + 1); i++) {
		if (scores[i] < 22 && scores[i] > max_points) {
			max_points = scores[i];
			max_player = i;
		}
	} //todo: 如果兩個玩家同點數，該如何處理？網路上查不到規則，所以這裏只取第一個最高分玩家
	printf("\n**************************************\n");
	printf("\t比賽結果：");
	
	if (max_points == 0) { game_result = 999; } ////莊家及玩家全部爆牌，平手
	else {
		if (max_player == 0) { game_result = 0; } //莊家贏 不爆牌而平手也算莊家贏
		else { game_result = max_player; }
	}
	//輸出結果
	if (game_result == 999) { printf("平手，沒有贏家！\n"); }
	if (game_result == 0) { printf("莊家贏！\n"); }
	if (game_result > 0 && game_result < 999) { printf("玩家%d贏！\n", game_result); }
	printf("\n**************************************\n");
	return game_result;
}

int main(int argc, char *argv[])
{
	
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //控制terminal用
	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	COORD coordCur = csbi.dwCursorPosition; //控制游標位置
	
	
	WSADATA wsa;
	SOCKET master, new_socket, client_socket[4], s;
	struct sockaddr_in server, address;
	
	
	int max_clients = 2, activity, addrlen, valread;
	char message[] = "歡迎來到黑傑克賭坊！ \r\n";
	char sorry_message[] = "對不起，已滿桌！";
	char warn_message[] = "傳送錯誤啟始數值！";
	int cards[52] = { 0 };  //只用一副牌，記錄洗牌後的順序
	int client_cards[5][5] = { 0 }; //記錄每個人的手牌。最多四個玩家加上莊家(client_cards[0][x])，每人最多五張牌。
	int start_sign, connected_player_num = 0, stand[4] = { 0 },total_stand=0; //stand記錄玩家是否已經停止要牌
	int i, m, m_count ,iResult;//dummy vars
	int game_stage = 0; //0-->開始進行遊戲第0回合，先拿了兩張牌；1-->開始進行遊戲第1回合，可補牌或停牌，依此類推
	int card_count = 0; //記錄目前發到第幾張牌
	int game_end = 0; //如果全部玩家停牌或者已經到第四回合結束(滿五張牌)則為1，開始計算勝負
	int n[5] = { 0 };//記錄每個人手中最後牌數，莊家是n[0];
	int scores[5],game_result; //game_result=999：莊家與玩家都爆牌，算平手，game_result=0 莊家贏；game_result=1~4代表贏的玩家編號
	
	int MAXRECV = 2048; //size of our receive buffer, this is string length.
	fd_set readfds; //set of socket descriptors
	char *buffer; //1 extra for null character, string termination
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	shuffle(cards); //洗牌

	for (i = 0; i < 4; i++) //最多四個client，也就是 max_clients <=4
	{
		client_socket[i] = 0;
	}

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialised.\n");

	//Create a socket
	if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(10000);

	//Bind
	if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(master, 5);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	addrlen = sizeof(struct sockaddr_in);
	
		
	
	printf("等待玩家連線.....\n");
	while (TRUE) {
		
		FD_ZERO(&readfds); //clear the socket fd set		
		FD_SET(master, &readfds); //add master socket to fd set
		//add child sockets to fd set
		for (i = 0; i < max_clients; i++)
		{
			s = client_socket[i];
			if (s > 0)	{FD_SET(s, &readfds);}
		}
		//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
		activity = select(0, &readfds, NULL, NULL, NULL);
		//If something happened on the master socket , then its an incoming connection 如果是master socket，表示新連線待處理
		if (FD_ISSET(master, &readfds))
		{
			if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			

			m = recv(new_socket, (char*)&start_sign, sizeof(start_sign), 0);
			if (connected_player_num >= max_clients){ 
				send(new_socket, sorry_message, sizeof(sorry_message), 0); 
			}
			
			if (start_sign == 99 && connected_player_num < max_clients ) {
				//inform user of socket number - used in send and receive commands
				printf("\nNew connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
				connected_player_num++;
				printf("第%d個玩家請求開始遊戲\n",connected_player_num);
				//送出歡迎訊息
				send(new_socket, message, sizeof(message), 0);
				send(new_socket, (char*)&connected_player_num, sizeof(connected_player_num), 0);
				//recv(new_socket, (char*)&m_count, sizeof(m_count), 0); 

				for (i = 0; i < max_clients; i++) //依序掃描到空的socket，再填入
				{
					if (client_socket[i] == 0)
					{
						client_socket[i] = new_socket; //new_socket 是socket fd
						printf("Adding to list of sockets at index %d \n", i);
						break; //所以跳出時的i代表第(i+1)個玩家
					}
				}
				printf("給玩家%d兩張牌\n", i + 1);
				client_cards[i + 1][0] = cards[card_count];
				n[i+1] = n[i+1] + 1;
				send(new_socket, (char*)&cards[card_count], sizeof(int), 0);
				client_cards[i + 1][1] = cards[card_count+1];
				n[i + 1] = n[i + 1] + 1;
				send(new_socket, (char*)&cards[card_count], sizeof(int), 0);
				card_count += 2;
				printf("傳送給玩家%d遊戲人數\n", max_clients);
				send(new_socket, (char*)&max_clients, sizeof(max_clients), 0);
			}
			else {
				if (start_sign != 99) { 
					printf("玩家傳送錯誤數值 %d\n", start_sign);
					send(new_socket, warn_message, sizeof(warn_message), 0); }
				else { send(new_socket, sorry_message, sizeof(sorry_message), 0); }
				closesocket(new_socket);
			}
			printf("\r目前連線人數：%d", connected_player_num);
		}
		//printf("\r目前連線人數：%d", connected_player_num);
		//printf("目前回合：%d\n", game_stage);
		if (connected_player_num == max_clients && game_stage ==0) {
			printf("\n進入遊戲，莊家抽牌是：");
			game_stage = 1; //進入第一回合
			client_cards[0][0] = cards[card_count]; //莊家抽兩張牌
			print_card(client_cards[0][0]);
			client_cards[0][1] = cards[card_count+1];
			print_card(client_cards[0][1]);
			card_count = card_count + 2;
			n[0] = 2;
			scores[0] = score_calculation(client_cards, 0);
			printf("  一共%d點\n", scores[0]);
			printf("\n");
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				send(s, (char*)&game_stage, sizeof(game_stage), 0);
				send(s, (char*)client_cards, sizeof(client_cards), 0);
			}
			//接收玩家選擇
			while (game_end == 0) {
				for (i = 0; i < max_clients; i++) {
					if (stand[i] == 0) {
						s = client_socket[i];
						//m = 0;
						iResult = recv(s, (char*)&m, sizeof(m), 0);
						if (m == 0) {
							printf("玩家%d選擇補牌\n", i+1);
							send(s, (char*)&cards[card_count], sizeof(int), 0);
							client_cards[i + 1][n[i + 1]] = cards[card_count];
							card_count++;
							n[i + 1] = n[i + 1] + 1;
						}
						else
						{
							printf("玩家%d停牌\n", i+1);
							stand[i] = 1;
							total_stand++;
						}
					}
				}
				//收完一輪玩家選擇，送出最新所有人牌組，以便重繪牌桌
				for (i = 0; i < max_clients; i++) {
					s = client_socket[i];
					send(s, (char*)client_cards, sizeof(client_cards), 0);
				}
				if (total_stand == max_clients) {
					printf("所有玩家都已停牌！\n");
					game_end = 1; 
				}
				
			} //while(game_end ==0)
			//printf("game_end=%d\n", game_end);
			//莊家如果點數小於17必須連續抽牌
			printf("莊家抽牌：\n");
			while (scores[0] < 17) {
				client_cards[0][n[0]] = cards[card_count];
				card_count++;
				n[0] = n[0] + 1;
				scores[0] = score_calculation(client_cards, 0);
			}
			//開始結算，送出狀態及牌組
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				send(s, (char*)&game_end, sizeof(game_end), 0);
			}
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				send(s, (char*)client_cards, sizeof(client_cards), 0);
			}

			printf("莊家手牌：");
			print_all_card(client_cards, 0);
			printf("  一共%d點\n", scores[0]);
			printf("所有玩家的手牌：\n");
			printf("一共%d個玩家\n", max_clients);
			for (i = 0; i < max_clients; i++) {
				printf("玩家%d的手牌有%d張：\n", i + 1,n[i+1]);
				print_all_card(client_cards, i + 1);
				//for (j = 0; j < n[i+1]; j++) {print_card(client_cards[i+1][j]);}
				scores[i + 1] = score_calculation(client_cards, i + 1); //注意二維陣列傳送必須指定行的數目
				printf("  一共%d點\n", scores[i + 1]);
			}
			//計算輸贏
			game_result=output_game_result(scores, max_clients);
		}
			   
		
		
		

		} //for while(TRUE) 太多括號


	
	
	




	closesocket(s);
	closesocket(master);
	WSACleanup();
	system("PAUSE");
	return 0;
}