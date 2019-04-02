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

	//�p��@�@�X�i�P
	for (i = 0; i < 5; i++) {
		if (a[i] != 0) { tot_card_num++; }
	}
	//�Ĥ@�C
	for (i = 0; i < tot_card_num; i++) {printf("*******  ");}
	printf("\n");
	//�ĤG�C
	for (i = 0; i < tot_card_num; i++) { printf("*     *  "); }
	printf("\n");
	//�ĤT�C
	for (i = 0; i < tot_card_num; i++) {
		fl = (a[i] - 1) / 13 + 3;
		if (i > 0 && mask == 1) {printf("*  *  *  ");}
		else { printf("* %c   *  ", fl); }
	}
	printf("\n");
	//�ĥ|�C
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
	//�Ĥ��C
	for (i = 0; i < tot_card_num; i++) { printf("*     *  "); }
	printf("\n");
	//�Ĥ��C
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
	if (mask == 1){ printf("           ���a                    \n"); }
	else {
		score = score_personal(slice_a);
		printf("           ���a�@�@%d�I             \n",score);
	}
	
	printf("===================================\n");
	
	if (mask ==1) { pic(slice_a, 1); }
	else { pic(slice_a, 5); }
	
	for (int i = 0; i < max_clients; i++) {
		slice_a = &a[i + 1][0];
		score = score_personal(slice_a);
		printf("===================================\n");
		if ((i + 1) == player_num) {
			printf("       ���a%d �@�@%d�I\n", i + 1, score);
		}
		else {
			if (mask == 1) { printf("       ���a%d \n", i + 1); }
			else {
				score = score_personal(slice_a);
				printf("       ���a%d �@�@%d�I\n", i + 1, score);
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
	int max_player = 0; //�O��Ĺ�a�s��
	int game_result=0;
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
	if (game_result == my_player_num) { printf("\t���ߡI�AĹ�F�I\n"); }
	else {
		if (game_result != 999) { printf("\t�藍�_�I�A��F�I\n"); }
	}
	printf("\n**************************************\n");
	return game_result;
}



int main(int argc, char *argv[])
{


	int my_game_stage = 0, server_game_stage = 0, game_end=0; //�ثe�ĴX�^�X�A�H�άO�_�w����
	int stand = 0; //�O�_�M�w�F����n�P�A�w�]��0��ܩ|������n�P
	int my_card[5] = { 0 };
	char answer = 'K'; //�_�l�Ȩ�N�������O'y'��'n'
	int start_sign = 99, end_sign = 1999;//�n�D�}�l/�����C��
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
	printf("�s�����A��.... \n");
	start_sign = 99;
	iResult = send(hServer, (char*)&start_sign, sizeof(start_sign), 0);//�e�X�n�D�}�l�C��
	if (iResult == SOCKET_ERROR) {
		printf("send failed!\n");
	}
	//printf("Bytes Sent: %ld\n", iResult);
	recv(hServer, recv_buff, sizeof(recv_buff), 0);
	//�p�G�w�W�L�|�H�s�u���A���A�j��h�X
	if (strcmp(recv_buff, "�藍�_�A�w����I") == 0) {
		printf(recv_buff);
		system("PAUSE");
		return EXIT_SUCCESS;
	}
	else {
		if (strcmp(recv_buff, "�ǰe���~�ҩl�ƭȡI") == 0) {
			printf(recv_buff);
			system("PAUSE");
			return EXIT_SUCCESS;
		}
	}
	
	recv(hServer, (char*)&my_player_num, sizeof(my_player_num), 0);
	if (my_player_num >4 || my_player_num <0) {
		printf("�C���H�ơG%d\n", my_player_num);
		printf("�s�u���~�I�������������s�Ұʵ{��\n");
		system("PAUSE");
		return EXIT_SUCCESS;
	}
	else {
		printf("���a%d�z�n�I�w��Ө�³ǧJ��{�I\n", my_player_num);
		printf("���ݩҦ����a�s�u...\n");
		sprintf(terminal_title, "���a%d�C�������I", my_player_num);
		set_terminal_title(terminal_title);
		system("COLOR 2F");
	}
	//�������ӤH���e��i�P
	for (card_count = 0; card_count < 2; card_count++) {
		recv(hServer, (char*)&card, sizeof(card), 0);
		my_card[card_count] = card;
		Sleep(100);
	}
	//�����C���H��
	recv(hServer, (char*)&max_clients, sizeof(max_clients), 0);
	while (TRUE) {
		//����game_stage�ܼơAgame_stage=1�N��H�Ƥw���A�}�l�C��
		recv(hServer, (char*)&server_game_stage, sizeof(server_game_stage), 0);
		//����client_cards
		recv(hServer, (char*)client_cards, sizeof(client_cards), 0);
		if (my_game_stage == 0 && server_game_stage == 1) {
			printf("�C���}�l\n");
			Sleep(2000);
			pic_all(client_cards,my_player_num,max_clients,1);
			score = score_calculation(client_cards, my_player_num);
			printf("\n\n���a%d  �{�b��%d�i�P ",my_player_num,card_count);
			score = score_calculation(client_cards,my_player_num);
			printf("  �X�p�@%d�I", score);
			//todo: �o�����ӭn�ˬd���a�O�_�w�g����blackjack
			 //�]��for loop�ح� card_count++����~�P�O�A����for loop
			while(card_count < 5 && stand == 0) {
					ASK: printf("\n�O�_�n�ɵP�H(y/n)");
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
						printf("�п�J(y/n)�I\n");
						//break; //�n��������ӥh���a��
						goto ASK;
					}
					else {
						//����client_cards
						recv(hServer, (char*)client_cards, sizeof(client_cards), 0);
						pic_all(client_cards, my_player_num, max_clients,1);
						score = score_calculation(client_cards,my_player_num);
						if (score > 21) {
							printf("\n�A���P�z�F�I����A�ɵP�C\n");
							stand = 1;
							send(hServer, (char*)&stand, sizeof(stand), 0);
						}
						else {
							if (card_count == 5) {
								printf("\n��P�w�����i�A����A�ɵP�C\n");
								stand = 1;
								send(hServer, (char*)&stand, sizeof(stand), 0);
							}
							if (stand == 1) { printf("\n�A�w��ܰ��P�I�{�b���ݨ�L���a���P...\n"); }
						}
					}			
			} //while(card_count < 5 && stand == 0) 
			//�w�g���P�A���ݵ���
			while (game_end != 1) {
				recv(hServer, (char*)&game_end, sizeof(game_end), 0);
				//printf("\rgame_end=%d   ", game_end);
			}
			if (game_end == 1) {
				//printf("game_end=%d\n", game_end);
				printf("\n�Ҧ����a�w���P�A�}�l����\n");
				recv(hServer, (char*)client_cards, sizeof(client_cards), 0);
				//�p��]�t���a�b�������Z
				for (i = 0; i < (max_clients + 1); i++) {
					scores[i] = score_calculation(client_cards, i); //�`�N�G���}�C�ǰe�������w�檺�ƥ�
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
