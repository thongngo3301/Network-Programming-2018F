#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

int main() {
	const int _family = AF_INET;	// IPv4
	const int _type = SOCK_STREAM;	// TCP
	const int _protocol = 0;
	const int _bufferLength = 1024;
	const char *_terminateChar = "end";

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

	// Connect to server by using serverSocket
	int connCheck = connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (connCheck < 0) {
		perror("Connect error");
		return 1;
	}

	printf("Start sending messages to server. Send 'end' to stop.\n");

	// Send message from client to server and receive response
	char sendBuffer[_bufferLength];
	char receiveBuffer[_bufferLength];

	while (1) {
		printf("Message: ");
		fgets(sendBuffer, sizeof(sendBuffer), stdin);
		sendBuffer[strlen(sendBuffer) - 1] = '\0';

		if (strcmp(sendBuffer, _terminateChar) == 0) {
			break;
		}

		int sentMsgLen = strlen(sendBuffer) + 1;		
		write(serverSocket, sendBuffer, sentMsgLen);

		read(serverSocket, receiveBuffer, sizeof(receiveBuffer));
		printf("Response from server: %s\n", receiveBuffer);
	}

	close(serverSocket);

	return 0;
}
