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
#include <ctype.h>
#include "protocol.h"
#define PROTOPORT 27015
#define QLEN 5
#define BUFFERSIZE 512

void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}

void clearwinsock() {
#ifdef WIN32
	WSACleanup();
#endif
}

/*Addition*/
int add(int n1, int n2){
	return n1+n2;
}

/*Multiplication*/
int mult(int n1, int n2){
	return n1*n2;
}

/*Subtraction*/
int sub(int n1, int n2){
	return n1-n2;
}

/*Division*/
int division(int n1, int n2){
	return n1/n2;
}

int main(void) {
	int port = PROTOPORT;
#ifdef WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR) {
		errorhandler("Error at WSAStartup()\n");
		system("pause");
		return 0;
	}
#endif
	/*Creating the Server socket*/
	int socketserver;
	socketserver = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketserver < 0) {
		errorhandler("socket creation failed. \n");
		clearwinsock();
		system("pause");
		return -1;
	}
	struct sockaddr_in localaddress;
	memset(&localaddress, 0, sizeof(localaddress));
	localaddress.sin_family = AF_INET;
	localaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	localaddress.sin_port = htons(port);

	if (bind(socketserver, (struct sockaddr*) &localaddress,
			sizeof(localaddress)) < 0) {
		errorhandler("bind() failed. \n");
		clearwinsock();
		system("pause");
		return -1;
	}
	/*Waiting for the client*/
	if (listen(socketserver, QLEN) < 0) {
		errorhandler("listen() failed.\n");
		closesocket(socketserver);
		clearwinsock();
		system("pause");
		return -1;
	}

	struct sockaddr_in clientaddress;
	int socketclient;
	int clientsize;
	printf("Waiting for a client to connect...\n");
	while (1) {
		clientsize = sizeof(clientaddress);
		if ((socketclient = accept(socketserver,
				(struct sockaddr*) &clientaddress, &clientsize)) < 0) {
			errorhandler("accept() failed.\n");
			closesocket(socketclient);
			clearwinsock();
			system("pause");
			return 0;
		}
		/*Sending connection message to the Client*/
		printf("Connection established with %s", inet_ntoa(clientaddress.sin_addr));
		printf( ":%d \n", clientaddress.sin_port);
		char *sendbuf = "Connection established\n";
		int bufSize = strlen(sendbuf);
		if (send(socketclient, sendbuf, bufSize, 0) < 0) {
			errorhandler("send() failed.\n");
			closesocket(socketserver);
			clearwinsock();
			system("pause");
			return -1;
		}
		struct msgStruct msg;
		do{
			/*Receiving the operation*/
			int bytes_rcvd = 0;
			int bytes = (int) sizeof(struct msgStruct);
			printf("Receiving the operation...\n");
			bytes_rcvd = recv(socketclient, (struct msgStruct*) &msg, bytes, 0);
			if (bytes_rcvd <= 0) {
				errorhandler("recv() string failed.\n");
				closesocket(socketserver);
				clearwinsock();
				system("pause");
				return -1;
			}
			/*Sending the result to the Client*/
			if(msg.operator=='+'){
				msg.n1=add(msg.n1,msg.n2);
				printf("Addition done.\n");
			} else if(msg.operator=='-'){
				msg.n1=sub(msg.n1,msg.n2);
				printf("Subtraction done.\n");
			} else if(msg.operator=='*'){
				msg.n1=mult(msg.n1,msg.n2);
				printf("Multiplication done.\n");
			} else if(msg.operator=='/'){
				msg.n1=division(msg.n1,msg.n2);
				printf("Division done.\n");
			} else {
				msg.operator='.';
				printf("Operation failed.\n");
			}

			int bytes_to_send = sizeof(struct msgStruct);
			int iResult = send(socketclient, (struct msgStruct*) &msg, bytes_to_send, 0);
			if (iResult < 0) {
				errorhandler("send() operation failed.\n");
				closesocket(socketserver);
				clearwinsock();
				system("pause");
				return -1;

			}
		}while(msg.a==1);

	}
	/*Closing the socket*/
	clearwinsock();
	closesocket(socketserver);
	system("pause");
	return 0;
}
