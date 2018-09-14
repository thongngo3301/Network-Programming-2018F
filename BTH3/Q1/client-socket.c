#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<strings.h>
#include<stdlib.h>
#include<stdio.h>

int main() {
	const int _family = AF_INET;	// IPv4
	const int _type = SOCK_STREAM;	// TCP
	const int _protocol = 0;
	const int _bufferLength = 1024;

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

	serverAddr.sin_addr.s_addr = inet_addr(serverIpAddr);
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_family = _family;

	// Connect to server by using serverSocket
	int connCheck = connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (connCheck < 0) {
		perror("Connect error");
		return 1;
	}

	int nbytes;

	// Send message from client to server
	char msg[] = "Hello Server!";
	nbytes = write(serverSocket, msg, sizeof(msg));
	if (nbytes < 0) {
		perror("Write error");
		return 1;
	}

	// Receive message from server to client
	char buffer[_bufferLength];
	nbytes = read(serverSocket, buffer, sizeof(buffer));
	if (nbytes < 0) {
		perror("Read error");
		return 1;
	}
	printf("Message from server: %s\n", buffer);

	close(serverSocket);
	printf("Closed connection.\n");

	return 0;
}
