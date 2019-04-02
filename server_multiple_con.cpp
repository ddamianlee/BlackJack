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
	for (int i = 1; i < 53; i++) { //�G�N���q0�}�l�A�H�K�ŵP�P�Ĥ@�i�P���O�s
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
	int max_player = 0; //�O��Ĺ�a�s��
	int game_result;
	//scores[0]�]�t���a�A�ҥHi < ( max_clients+1)
	for (int i = 0; i < (max_clients + 1); i++) {
		if (scores[i] < 22 && scores[i] > max_points) {
			max_points = scores[i];
			max_player = i;
		}
	} //todo: �p�G��Ӫ��a�P�I�ơA�Ӧp��B�z�H�����W�d����W�h�A�ҥH�o�إu���Ĥ@�ӳ̰������a
	printf("\n**************************************\n");
	printf("\t���ɵ��G�G");
	
	if (max_points == 0) { game_result = 999; } ////���a�Ϊ��a�����z�P�A����
	else {
		if (max_player == 0) { game_result = 0; } //���aĹ ���z�P�ӥ���]����aĹ
		else { game_result = max_player; }
	}
	//��X���G
	if (game_result == 999) { printf("����A�S��Ĺ�a�I\n"); }
	if (game_result == 0) { printf("���aĹ�I\n"); }
	if (game_result > 0 && game_result < 999) { printf("���a%dĹ�I\n", game_result); }
	printf("\n**************************************\n");
	return game_result;
}

int main(int argc, char *argv[])
{
	
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //����terminal��
	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	COORD coordCur = csbi.dwCursorPosition; //�����Ц�m
	
	
	WSADATA wsa;
	SOCKET master, new_socket, client_socket[4], s;
	struct sockaddr_in server, address;
	
	
	int max_clients = 2, activity, addrlen, valread;
	char message[] = "�w��Ө�³ǧJ��{�I \r\n";
	char sorry_message[] = "�藍�_�A�w����I";
	char warn_message[] = "�ǰe���~�ҩl�ƭȡI";
	int cards[52] = { 0 };  //�u�Τ@�ƵP�A�O���~�P�᪺����
	int client_cards[5][5] = { 0 }; //�O���C�ӤH����P�C�̦h�|�Ӫ��a�[�W���a(client_cards[0][x])�A�C�H�̦h���i�P�C
	int start_sign, connected_player_num = 0, stand[4] = { 0 },total_stand=0; //stand�O�����a�O�_�w�g����n�P
	int i, m, m_count ,iResult;//dummy vars
	int game_stage = 0; //0-->�}�l�i��C����0�^�X�A�����F��i�P�F1-->�}�l�i��C����1�^�X�A�i�ɵP�ΰ��P�A�̦�����
	int card_count = 0; //�O���ثe�o��ĴX�i�P
	int game_end = 0; //�p�G�������a���P�Ϊ̤w�g��ĥ|�^�X����(�����i�P)�h��1�A�}�l�p��ӭt
	int n[5] = { 0 };//�O���C�ӤH�⤤�̫�P�ơA���a�On[0];
	int scores[5],game_result; //game_result=999�G���a�P���a���z�P�A�⥭��Agame_result=0 ���aĹ�Fgame_result=1~4�N��Ĺ�����a�s��
	
	int MAXRECV = 2048; //size of our receive buffer, this is string length.
	fd_set readfds; //set of socket descriptors
	char *buffer; //1 extra for null character, string termination
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	shuffle(cards); //�~�P

	for (i = 0; i < 4; i++) //�̦h�|��client�A�]�N�O max_clients <=4
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
	
		
	
	printf("���ݪ��a�s�u.....\n");
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
		//If something happened on the master socket , then its an incoming connection �p�G�Omaster socket�A��ܷs�s�u�ݳB�z
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
				printf("��%d�Ӫ��a�ШD�}�l�C��\n",connected_player_num);
				//�e�X�w��T��
				send(new_socket, message, sizeof(message), 0);
				send(new_socket, (char*)&connected_player_num, sizeof(connected_player_num), 0);
				//recv(new_socket, (char*)&m_count, sizeof(m_count), 0); 

				for (i = 0; i < max_clients; i++) //�̧Ǳ��y��Ū�socket�A�A��J
				{
					if (client_socket[i] == 0)
					{
						client_socket[i] = new_socket; //new_socket �Osocket fd
						printf("Adding to list of sockets at index %d \n", i);
						break; //�ҥH���X�ɪ�i�N���(i+1)�Ӫ��a
					}
				}
				printf("�����a%d��i�P\n", i + 1);
				client_cards[i + 1][0] = cards[card_count];
				n[i+1] = n[i+1] + 1;
				send(new_socket, (char*)&cards[card_count], sizeof(int), 0);
				client_cards[i + 1][1] = cards[card_count+1];
				n[i + 1] = n[i + 1] + 1;
				send(new_socket, (char*)&cards[card_count], sizeof(int), 0);
				card_count += 2;
				printf("�ǰe�����a%d�C���H��\n", max_clients);
				send(new_socket, (char*)&max_clients, sizeof(max_clients), 0);
			}
			else {
				if (start_sign != 99) { 
					printf("���a�ǰe���~�ƭ� %d\n", start_sign);
					send(new_socket, warn_message, sizeof(warn_message), 0); }
				else { send(new_socket, sorry_message, sizeof(sorry_message), 0); }
				closesocket(new_socket);
			}
			printf("\r�ثe�s�u�H�ơG%d", connected_player_num);
		}
		//printf("\r�ثe�s�u�H�ơG%d", connected_player_num);
		//printf("�ثe�^�X�G%d\n", game_stage);
		if (connected_player_num == max_clients && game_stage ==0) {
			printf("\n�i�J�C���A���a��P�O�G");
			game_stage = 1; //�i�J�Ĥ@�^�X
			client_cards[0][0] = cards[card_count]; //���a���i�P
			print_card(client_cards[0][0]);
			client_cards[0][1] = cards[card_count+1];
			print_card(client_cards[0][1]);
			card_count = card_count + 2;
			n[0] = 2;
			scores[0] = score_calculation(client_cards, 0);
			printf("  �@�@%d�I\n", scores[0]);
			printf("\n");
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				send(s, (char*)&game_stage, sizeof(game_stage), 0);
				send(s, (char*)client_cards, sizeof(client_cards), 0);
			}
			//�������a���
			while (game_end == 0) {
				for (i = 0; i < max_clients; i++) {
					if (stand[i] == 0) {
						s = client_socket[i];
						//m = 0;
						iResult = recv(s, (char*)&m, sizeof(m), 0);
						if (m == 0) {
							printf("���a%d��ܸɵP\n", i+1);
							send(s, (char*)&cards[card_count], sizeof(int), 0);
							client_cards[i + 1][n[i + 1]] = cards[card_count];
							card_count++;
							n[i + 1] = n[i + 1] + 1;
						}
						else
						{
							printf("���a%d���P\n", i+1);
							stand[i] = 1;
							total_stand++;
						}
					}
				}
				//�����@�����a��ܡA�e�X�̷s�Ҧ��H�P�աA�H�K��ø�P��
				for (i = 0; i < max_clients; i++) {
					s = client_socket[i];
					send(s, (char*)client_cards, sizeof(client_cards), 0);
				}
				if (total_stand == max_clients) {
					printf("�Ҧ����a���w���P�I\n");
					game_end = 1; 
				}
				
			} //while(game_end ==0)
			//printf("game_end=%d\n", game_end);
			//���a�p�G�I�Ƥp��17�����s���P
			printf("���a��P�G\n");
			while (scores[0] < 17) {
				client_cards[0][n[0]] = cards[card_count];
				card_count++;
				n[0] = n[0] + 1;
				scores[0] = score_calculation(client_cards, 0);
			}
			//�}�l����A�e�X���A�εP��
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				send(s, (char*)&game_end, sizeof(game_end), 0);
			}
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				send(s, (char*)client_cards, sizeof(client_cards), 0);
			}

			printf("���a��P�G");
			print_all_card(client_cards, 0);
			printf("  �@�@%d�I\n", scores[0]);
			printf("�Ҧ����a����P�G\n");
			printf("�@�@%d�Ӫ��a\n", max_clients);
			for (i = 0; i < max_clients; i++) {
				printf("���a%d����P��%d�i�G\n", i + 1,n[i+1]);
				print_all_card(client_cards, i + 1);
				//for (j = 0; j < n[i+1]; j++) {print_card(client_cards[i+1][j]);}
				scores[i + 1] = score_calculation(client_cards, i + 1); //�`�N�G���}�C�ǰe�������w�檺�ƥ�
				printf("  �@�@%d�I\n", scores[i + 1]);
			}
			//�p���Ĺ
			game_result=output_game_result(scores, max_clients);
		}
			   
		
		
		

		} //for while(TRUE) �Ӧh�A��


	
	
	




	closesocket(s);
	closesocket(master);
	WSACleanup();
	system("PAUSE");
	return 0;
}