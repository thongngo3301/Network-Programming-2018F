#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<strings.h>
#include<stdio.h>

int main() {
	const int _family = AF_INET;	// IPv4
	const int _type = SOCK_STREAM;	// TCP
	const int _protocol = 0;
	const int _port = 9090;
	const int _listenQueue = 1024;	// Backlog in listen()
	const int _bufferLength = 1024;

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

	unsigned int addrLength = sizeof(connClientAddr);
	connClientSocket = accept(serverSocket, (struct sockaddr *) &connClientAddr, &addrLength);
	if (connClientSocket < 0) {
		perror("Accept error");
		return 1;
	}

	// Get client info
	char *clientIpAddr = inet_ntoa(connClientAddr.sin_addr);
	int clientPort = ntohs(connClientAddr.sin_port);

	printf("Client address: %s:%d\n", clientIpAddr, clientPort);

	int nbytes;

	// Receive message from client to server
	char buffer[_bufferLength];
	nbytes = read(connClientSocket, buffer, sizeof(buffer));
	if (nbytes < 0) {
		perror("Read error");
		return 1;
	}
	printf("Message from client: %s\n", buffer);

	// Send message from server back to client
	char msg[] = "Hello Client!";
	nbytes = write(connClientSocket, msg, sizeof(msg));
	if (nbytes < 0) {
		perror("Write error");
		return 1;
	}

	close(connClientSocket);
	close(serverSocket);

	return 0;
}
