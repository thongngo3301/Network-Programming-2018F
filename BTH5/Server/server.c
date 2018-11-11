/**
 * Name: Ngo Doan Thong
 * MSSV: 16022451
 * Description:
 *    - Server open port to listen to connections from Clients
 *    - Server receive file name from Clients
 *    - Server send file back to Clients
 *    - When receive terminate character, Server close connection
 **/

#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/select.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>

const int _listenQueue = 1024;	// Backlog in listen()
const int _bufferLength = 1024;
const char *_terminateChar = "QUIT";

int nFile = 0;

int handleRequest(int connClientSocket) {
    // Receive file name from client and send file back
    char buffer[_bufferLength];
    char fileName[_bufferLength];

    // Read the requested file name
    if (read(connClientSocket, fileName, sizeof(fileName)) <= 0) {
        return 0;
    }

    if (strcmp(fileName, _terminateChar) == 0) {
        return 0;
    }

    // Get the requested file
    FILE *file = fopen(fileName, "rb");
    int size;
    // Handle error
    if (file == NULL) {
        size = 0;
        printf("Cannot find file '%s'!\n", fileName);
        write(connClientSocket, (void *) &size, sizeof(int));
        return 0;
    }
    printf("File '%s' is being sent...\n", fileName);

    // Send file length first
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("File size: %d bytes\n", size);
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
    nFile++;
    printf("Total files sent to Clients: %d\n", nFile);
    fclose(file);
    return 1;
}

int main() {
    const int _family = AF_INET;	// IPv4
    const int _type = SOCK_STREAM;	// TCP
    const int _protocol = 0;
    const int _port = 9090;

    int serverSocket, fd;
    struct sockaddr_in serverAddr, connClientAddr;

    // Create a socket to listen
    serverSocket = socket(_family, _type, _protocol);
    if (serverSocket < 0) {
        perror("Server socket error");
        return 1;
    }
    
    int on = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

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
    
    fd_set rfds;                    // Read file descriptor set
    fd_set afds;                    // Active file descriptor set

    FD_ZERO(&afds);
    FD_SET(serverSocket, &afds);

    while (1) {
        memcpy(&rfds, &afds, sizeof(rfds));

        if (select(FD_SETSIZE, &rfds, (fd_set *)0, (fd_set*)0, (struct timeval *)0) < 0) {
            perror("Select error");
            return 1;
        }

        if (FD_ISSET(serverSocket, &rfds)) {
            int cliSockFd = accept(serverSocket, (struct sockaddr *) &connClientAddr, &addrLength);;
            if (cliSockFd < 0) {
                perror("Accept error");
                continue;   
            }
            // Get client info
            char *clientIpAddr = inet_ntoa(connClientAddr.sin_addr);
            int clientPort = ntohs(connClientAddr.sin_port);
            printf("Client address: %s:%d\n", clientIpAddr, clientPort);
            
            FD_SET(cliSockFd, &afds);
        }

        for (fd = 0; fd < FD_SETSIZE; ++fd) {
            if (fd != serverSocket && FD_ISSET(fd, &rfds)) {
                if (handleRequest(fd) == 0) {
                    close(fd);
                    FD_CLR(fd, &afds);
                    break;
                }
            }
        }        
    }

    return 0;
}
