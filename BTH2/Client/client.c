/**
 * Name: Ngo Doan Thong
 * MSSV: 16022451
 * Description:
 *    - User type server's IP address and port from keyboard
 *    - User type message from keyboard
 *    - Client read and send message to Server
 *    - Client display response from Server
 *    - User type 'end' to close connection
**/

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

int main() {
	const int _family = AF_INET;	// IPv4
	const int _type = SOCK_STREAM;	// TCP
	const int _protocol = 0;
	const int _bufferLength = 1024;
	const char *_terminateChar = "QUIT";

	int serverSocket;
	struct sockaddr_in serverAddr;

	char* serverIpAddr = (char *) malloc(100 * sizeof(char *));
	int serverPort;

	serverSocket = socket(_family, _type, _protocol);
	if (serverSocket < 0) {
		perror("Server socket error");
		return 1;
	}

	bzero(&serverAddr, sizeof(serverAddr));

	printf("Server IP Address: ");
	scanf("%s", serverIpAddr);
	printf("Server Port: ");
	scanf("%d", &serverPort);
	getchar();

	serverAddr.sin_addr.s_addr = inet_addr(serverIpAddr);
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_family = _family;

	setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (void *)&_bufferLength, sizeof(_bufferLength));

	// Connect to server by using serverSocket
	int connCheck = connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (connCheck < 0) {
		perror("Connect error");
		return 1;
	}

	printf("Connected to server.\n");

	while (1) {
		printf("Enter file name to download, send 'QUIT' to stop: ");

		// Send file name to server and receive file from server
		char sendBuffer[_bufferLength];
		char receiveBuffer[_bufferLength];
		unsigned int fileSize = 0;
		fgets(sendBuffer, sizeof(sendBuffer), stdin);
		sendBuffer[strlen(sendBuffer) - 1] = '\0';

		if (strcmp(sendBuffer, _terminateChar) == 0) {
			break;
		}

		int sentMsgLen = strlen(sendBuffer) + 1;		
		write(serverSocket, sendBuffer, sentMsgLen);

		// Receive file size
		unsigned int receivedSize = 0;
		read(serverSocket, &fileSize, sizeof(fileSize));
		// Receive file
		FILE *file = fopen(sendBuffer, "wb");
		while (receivedSize < fileSize) {
			int currRcvSize = read(serverSocket, receiveBuffer, _bufferLength);
			receivedSize += currRcvSize;
			fwrite(receiveBuffer, 1, currRcvSize, file);
		}
		printf("Received file '%s' successfully!\n", sendBuffer);
		printf("Total received file size: %d\n", receivedSize);
		fclose(file);
	}

	close(serverSocket);
	printf("Closed connection.\n");

	return 0;
}
