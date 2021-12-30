#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PROTOPORT 27015
#define BUFFERSIZE 512
#include "protocol.h"

void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}
void clearwinsock() {
#ifdef WIN32
	WSACleanup();
#endif
}

int main(void) {
#ifdef WIN32
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR) {
		errorhandler("Error at WSAStartup().\n");
		system("pause");
		return -1;
	}
#endif

	/*Creation of the Client Socket*/
	int socketclient;
	socketclient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketclient < 0) {
		errorhandler("Socket creation failed.\n");
		closesocket(socketclient);
		clearwinsock();
		system("pause");
		return -1;
	}
	struct sockaddr_in serveraddress;
	memset(&serveraddress, 0, sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;

	/*Request to enter the Server address*/
	char ipAddress[12];
	printf("Enter the IP address: ");
	scanf("%s", ipAddress);
	serveraddress.sin_addr.s_addr = inet_addr(ipAddress);
	int serverPort;
	printf("Enter the Server Port: ");
	scanf("%d", &serverPort);
	serveraddress.sin_port = htons(serverPort);

	/*Test connection to server*/
	if (connect(socketclient, (struct sockaddr*) &serveraddress,(int) sizeof(serveraddress)) < 0) {
		errorhandler("Failed to connect.\n");
		closesocket(socketclient);
		clearwinsock();
		system("pause");
			return -1;
	}

	/*Waiting to receive the connection message*/
	int bytes_rcvd;
	char buffer[BUFFERSIZE];
	printf("Received: ");
	if ((bytes_rcvd = recv(socketclient, buffer, BUFFERSIZE - 1, 0)) <= 0) {
		errorhandler("recv() failed or connection closed prematurely.\n");
		closesocket(socketclient);
		clearwinsock();
		system("pause");
		return -1;
	}
	buffer[bytes_rcvd] = '\0';
	printf("%s", buffer);
	char c=getchar();

	/*Request for insertion of the operation to the user and sending it to the Server*/
	while(1){
		printf("Enter the operation:");
		struct msgStruct msg;
		char st[20];
		fgets(st, 20, stdin);
		char *ch;
		ch = strtok(st, " ");
		msg.operator=ch[0];
	    ch = strtok(NULL, " ");
	    msg.n1=atoi(ch);
	    ch = strtok(NULL, " ");
	    msg.n2=atoi(ch);

		if(msg.operator == '+' || msg.operator == '-' || msg.operator == '*' || (msg.operator == '/' && msg.n2!=0)){
			msg.a=1;
			int bytes = (int) sizeof(struct msgStruct);
			if (send(socketclient, (struct msgStruct*) &msg, bytes, 0) < 0) {
				errorhandler("send() operation failed.\n");
				closesocket(socketclient);
				clearwinsock();
				system("pause");
				return -1;
			}
			printf("Operation sended.\n");

			/*Waiting the result from the Server*/
			int bytes_to_receive = (int) sizeof(struct msgStruct);
			int iResult = recv(socketclient, (struct msgStruct*) &msg, bytes_to_receive,0);
			if (iResult <= 0) {
				errorhandler("recv() operation failed.\n");
				closesocket(socketclient);
				clearwinsock();
				system("pause");
				return -1;
			}
			if(msg.operator=='.'){
				printf("Operation failed.\n");
			} else {
				printf("The result is %d.\n",msg.n1);
			}

		}else if(msg.operator == '='){
			msg.a=0;
			int bytes = (int) sizeof(struct msgStruct);
			if (send(socketclient, (struct msgStruct*) &msg, bytes, 0) < 0) {
				errorhandler("send() operation failed.\n");
				closesocket(socketclient);
				clearwinsock();
				system("pause");
				return -1;
			}
			/*Waiting the result from the Server*/
			int bytes_to_receive = (int) sizeof(struct msgStruct);
			int iResult = recv(socketclient, (struct msgStruct*) &msg, bytes_to_receive,0);
			if (iResult <= 0) {
				errorhandler("recv() operation failed.\n");
				closesocket(socketclient);
				clearwinsock();
				system("pause");
				return -1;
			}
			/*Closing the socket*/
			closesocket(socketclient);
			clearwinsock();
			return 0;
		} else {
			errorhandler("Wrong operator, re-enter data\n");
		}
	}

}
