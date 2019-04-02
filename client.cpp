#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <conio.h>
using namespace std;
#define _CRT_SECURE_NO_WARNINGS
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning( disable : 4996 34 ) 

void reset_terminal_cursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	COORD coordCur = csbi.dwCursorPosition;

	coordCur.X = coordCur.X+10;
	coordCur.Y = coordCur.Y-6;
	SetConsoleCursorPosition(hConsole, coordCur);
}
void set_terminal_title(char *title) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleTitle(TEXT("Win32 Console Control Demo"));
	SetConsoleTitle(TEXT(title));
	SMALL_RECT windowSize = { 0, 0, 79, 53 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}
void pic(int *a, int mask)
{
	int num,i,tot_card_num=0;
	char fl;
	int po_num;

	//計算一共幾張牌
	for (i = 0; i < 5; i++) {
		if (a[i] != 0) { tot_card_num++; }
	}
	//第一列
	for (i = 0; i < tot_card_num; i++) {printf("*******  ");}
	printf("\n");
	//第二列
	for (i = 0; i < tot_card_num; i++) { printf("*     *  "); }
	printf("\n");
	//第三列
	for (i = 0; i < tot_card_num; i++) {
		fl = (a[i] - 1) / 13 + 3;
		if (i > 0 && mask == 1) {printf("*  *  *  ");}
		else { printf("* %c   *  ", fl); }
	}
	printf("\n");
	//第四列
	for (i = 0; i < tot_card_num; i++) {
		po_num = a[i] % 13;
		if (i > 0 && mask == 1) { printf("*   * *  "); }
		else {
			switch (po_num) {
			case 1: {
				printf("*   A *  ");
				break;
			}
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10: {
				printf("*  %2d *  ", po_num);
				break;
			}
			case 11: {
				printf("*   J *  ");
				break;
			}
			case 12: {
				printf("*   Q *  ");
				break;
			}
			case 0: {
				printf("*   K *  ");
				break;
			}

			}
		}
	}
	printf("\n");
	//第五列
	for (i = 0; i < tot_card_num; i++) { printf("*     *  "); }
	printf("\n");
	//第六列
	for (i = 0; i < tot_card_num; i++) { printf("*******  "); }
	printf("\n");

}

void print_card(int card) {
	int card_suit = (card-1) / 13;
	int card_num = card % 13;
	switch (card_num) {
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

void print_all_card(int *a) {
	for (int i = 0; i < 5; i++) {
		if (a[i] != 0) {
			print_card(a[i]);
		}
	}
}
void print_all_player_card(int a[][5], int num) {
	for (int i = 0; i < 5; i++) {
		if (a[num][i] != 0) {
			print_card(a[num][i]);
			//pic(a[num][i]);
		}
	}
}
int score_calculation(int a[][5], int num) {
	int score = 0, temp = 0;
	for (int i = 0; i < 5; i++) {
		temp = a[num][i] % 13;
		if (temp == 0 && a[num][i] > 0) { temp = 13; }
		if (temp > 10) { temp = 10; }
		score += temp;
	}
	return score;
}
int score_personal(int *a) {
	int score = 0, temp = 0;
	for (int i = 0; i < 5; i++) {
		temp = a[i] % 13;
		if (temp == 0 && a[i] > 0) { temp = 13; }
		if (temp > 10) { temp = 10; }
		score += temp;
	}
	return score;
}
void pic_all(int a[][5], int player_num, int max_clients, int mask) {
	int *slice_a,score;
	system("CLS");
	slice_a = &a[0][0];
	printf("===================================\n");
	if (mask == 1){ printf("           莊家                    \n"); }
	else {
		score = score_personal(slice_a);
		printf("           莊家一共%d點             \n",score);
	}
	
	printf("===================================\n");
	
	if (mask ==1) { pic(slice_a, 1); }
	else { pic(slice_a, 5); }
	
	for (int i = 0; i < max_clients; i++) {
		slice_a = &a[i + 1][0];
		score = score_personal(slice_a);
		printf("===================================\n");
		if ((i + 1) == player_num) {
			printf("       玩家%d 一共%d點\n", i + 1, score);
		}
		else {
			if (mask == 1) { printf("       玩家%d \n", i + 1); }
			else {
				score = score_personal(slice_a);
				printf("       玩家%d 一共%d點\n", i + 1, score);
			}
		}
		printf("===================================\n");
		if (mask == 1) {
			if ((i + 1) == player_num) {pic(slice_a, 5);}
			else {pic(slice_a, 1);}
		}
		else { pic(slice_a, 5); }
	}
}
int output_game_result(int *scores, int max_clients,int my_player_num) {
	int max_points = 0;
	int max_player = 0; //記錄贏家編號
	int game_result=0;
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
	if (game_result == my_player_num) { printf("\t恭喜！你贏了！\n"); }
	else {
		if (game_result != 999) { printf("\t對不起！你輸了！\n"); }
	}
	printf("\n**************************************\n");
	return game_result;
}



int main(int argc, char *argv[])
{


	int my_game_stage = 0, server_game_stage = 0, game_end=0; //目前第幾回合，以及是否已結束
	int stand = 0; //是否決定了停止要牌，預設為0表示尚未停止要牌
	int my_card[5] = { 0 };
	char answer = 'K'; //起始值刻意讓它不是'y'或'n'
	int start_sign = 99, end_sign = 1999;//要求開始/結束遊戲
	int card,  card_count,score,scores[5];
	char recv_buff[200], terminal_title[100]; 
	int iResult;
	int client_cards[5][5] = { 0 }, max_clients,game_result;
	int my_player_num,i;
	//init
	WSADATA wsaData = { 0 };
	WORD wVer = MAKEWORD(2, 2);
	WSAStartup(wVer, &wsaData);

	//socket
	SOCKET hServer;
	hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	//connect
	sockaddr_in saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(10000);
	saServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	iResult = connect(hServer, (sockaddr*)&saServer, sizeof(sockaddr));
	if (iResult == SOCKET_ERROR) {
		printf("connection failed!\n");
	}
	//send,recv
	printf("連接伺服器.... \n");
	start_sign = 99;
	iResult = send(hServer, (char*)&start_sign, sizeof(start_sign), 0);//送出要求開始遊戲
	if (iResult == SOCKET_ERROR) {
		printf("send failed!\n");
	}
	//printf("Bytes Sent: %ld\n", iResult);
	recv(hServer, recv_buff, sizeof(recv_buff), 0);
	//如果已超過四人連線伺服器，強制退出
	if (strcmp(recv_buff, "對不起，已滿桌！") == 0) {
		printf(recv_buff);
		system("PAUSE");
		return EXIT_SUCCESS;
	}
	else {
		if (strcmp(recv_buff, "傳送錯誤啟始數值！") == 0) {
			printf(recv_buff);
			system("PAUSE");
			return EXIT_SUCCESS;
		}
	}
	
	recv(hServer, (char*)&my_player_num, sizeof(my_player_num), 0);
	if (my_player_num >4 || my_player_num <0) {
		printf("遊戲人數：%d\n", my_player_num);
		printf("連線錯誤！請關閉視窗重新啟動程式\n");
		system("PAUSE");
		return EXIT_SUCCESS;
	}
	else {
		printf("玩家%d您好！歡迎來到黑傑克賭坊！\n", my_player_num);
		printf("等待所有玩家連線...\n");
		sprintf(terminal_title, "玩家%d遊戲視窗！", my_player_num);
		set_terminal_title(terminal_title);
		system("COLOR 2F");
	}
	//先接收個人的前兩張牌
	for (card_count = 0; card_count < 2; card_count++) {
		recv(hServer, (char*)&card, sizeof(card), 0);
		my_card[card_count] = card;
		Sleep(100);
	}
	//接收遊戲人數
	recv(hServer, (char*)&max_clients, sizeof(max_clients), 0);
	while (TRUE) {
		//接收game_stage變數，game_stage=1代表人數已滿，開始遊戲
		recv(hServer, (char*)&server_game_stage, sizeof(server_game_stage), 0);
		//接收client_cards
		recv(hServer, (char*)client_cards, sizeof(client_cards), 0);
		if (my_game_stage == 0 && server_game_stage == 1) {
			printf("遊戲開始\n");
			Sleep(2000);
			pic_all(client_cards,my_player_num,max_clients,1);
			score = score_calculation(client_cards, my_player_num);
			printf("\n\n玩家%d  現在有%d張牌 ",my_player_num,card_count);
			score = score_calculation(client_cards,my_player_num);
			printf("  合計共%d點", score);
			//todo: 這裏應該要檢查玩家是否已經拿到blackjack
			 //因為for loop裏面 card_count++之後才判別，脫離for loop
			while(card_count < 5 && stand == 0) {
					ASK: printf("\n是否要補牌？(y/n)");
					scanf(" %c", &answer);
					if (answer == 'y') {
						stand = 0; //default value
						send(hServer, (char*)&stand, sizeof(stand), 0);
						recv(hServer, (char*)&card, sizeof(card), 0);
						card_count++;
						my_card[card_count] = card;	
					}
					if (answer == 'n') {
						stand = 1;
						send(hServer, (char*)&stand, sizeof(stand), 0);
					}
					if (answer != 'y' && answer != 'n') {
						printf("請輸入(y/n)！\n");
						//break; //好像跳不到該去的地方
						goto ASK;
					}
					else {
						//接收client_cards
						recv(hServer, (char*)client_cards, sizeof(client_cards), 0);
						pic_all(client_cards, my_player_num, max_clients,1);
						score = score_calculation(client_cards,my_player_num);
						if (score > 21) {
							printf("\n你的牌爆了！不能再補牌。\n");
							stand = 1;
							send(hServer, (char*)&stand, sizeof(stand), 0);
						}
						else {
							if (card_count == 5) {
								printf("\n手牌已滿五張，不能再補牌。\n");
								stand = 1;
								send(hServer, (char*)&stand, sizeof(stand), 0);
							}
							if (stand == 1) { printf("\n你已選擇停牌！現在等待其他玩家停牌...\n"); }
						}
					}			
			} //while(card_count < 5 && stand == 0) 
			//已經停牌，等待結算
			while (game_end != 1) {
				recv(hServer, (char*)&game_end, sizeof(game_end), 0);
				//printf("\rgame_end=%d   ", game_end);
			}
			if (game_end == 1) {
				//printf("game_end=%d\n", game_end);
				printf("\n所有玩家已停牌，開始結算\n");
				recv(hServer, (char*)client_cards, sizeof(client_cards), 0);
				//計算包含莊家在內的成績
				for (i = 0; i < (max_clients + 1); i++) {
					scores[i] = score_calculation(client_cards, i); //注意二維陣列傳送必須指定行的數目
				}
				Sleep(3000);
				pic_all(client_cards, my_player_num, max_clients, 0);
				
				game_result = output_game_result(scores, max_clients,my_player_num);

			}
		} //if (my_game_stage == 0 && server_game_stage == 1) 
		break;
	}




	//close
	closesocket(hServer);
	WSACleanup();

	system("PAUSE");
	return EXIT_SUCCESS;
}
