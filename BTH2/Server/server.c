/**
 * Name: Ngo Doan Thong
 * MSSV: 16022451
 * Description:
 *    - Server open port to listen to connection from Client
 *    - Server receive message from Client
 *    - Server change all characters in message into upper case
 *    - Server send modified message back to Client
 *    - When receive terminate character, Server close connection
**/

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<stdio.h>

int main() {
	const int _family = AF_INET;	// IPv4
	const int _type = SOCK_STREAM;	// TCP
	const int _protocol = 0;
	const int _port = 9090;
	const int _listenQueue = 1024;	// Backlog in listen()
	const int _bufferLength = 1024;
	const char *_terminateChar = "QUIT";

	int serverSocket, connClientSocket;
	struct sockaddr_in serverAddr, connClientAddr;

	// Create a socket to listen
	serverSocket = socket(_family, _type, _protocol);
	if (serverSocket < 0) {
		perror("Server socket error");
		return 1;
	}

	bzero(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(_port);
	serverAddr.sin_family = _family;

	int bindCheck = bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (bindCheck < 0) {
		perror("Bind error");
		return 1;
	}

	int listenCheck = listen(serverSocket, _listenQueue);
	if (listenCheck < 0) {
		perror("Listen error");
		return 1;
	}

	printf("Server is listening at port %d. Waiting for connection...\n", _port);

	unsigned int addrLength = sizeof(connClientAddr);

	while ((connClientSocket = accept(serverSocket, (struct sockaddr *) &connClientAddr, &addrLength)) >= 0) {
		// Get client info
		char *clientIpAddr = inet_ntoa(connClientAddr.sin_addr);
		int clientPort = ntohs(connClientAddr.sin_port);

		printf("Client address: %s:%d\n", clientIpAddr, clientPort);

		// Receive file name from client and send file back
		char buffer[_bufferLength];
		char fileName[_bufferLength];
		// Read the requested file name
		while (read(connClientSocket, fileName, sizeof(fileName)) > 0) {
			if (strcmp(fileName, _terminateChar) == 0) {
				break;
			}
			printf("File '%s' is being sent...\n", fileName);

			// Get the requested file
			FILE *file = fopen(fileName, "rb");
			int size;
			// Handle error
			if (file == NULL) {
				size = 0;
				write(connClientSocket, (void *) &size, sizeof(int));
				continue;
			}
			// Send file length first
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);
			printf("File size: %d\n", size);
			write(connClientSocket, (void *) &size, sizeof(int));
			// Send file
			while (!feof(file)) {
				// Read file to buffer
				int readSize = fread(buffer, 1, sizeof(buffer) - 1, file);
				write(connClientSocket, buffer, readSize);
				// Zero out buffer after writing
				bzero(buffer, sizeof(buffer));
			}
			printf("Sent file successfully!\n");
		}
		printf("Session ended.\n");
		printf("Server is listening at port %d. Waiting for connection...\n", _port);
		close(connClientSocket);

	}
	close(serverSocket);
	printf("Closed connection.\n");

	return 0;
}
