/**
 * Name: Ngo Doan Thong
 * MSSV: 16022451
 * Description:
 *    - User type server's IP address and port from keyboard
 *    - User type message from keyboard
 *    - Client read and send file name to Server
 *    - Client receive requested file from Server
 *    - User type 'end' to close connection
 **/

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>

int main() {
    const int _family = AF_INET;	// IPv4
    const int _type = SOCK_STREAM;	// TCP
    const int _protocol = 0;
    int _bufferLength;
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

    printf("Set buffer size: ");
    scanf("%d", &_bufferLength);

    setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (void *)&_bufferLength, sizeof(_bufferLength));
    unsigned int tmpSize = sizeof(_bufferLength);
    getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (void *)&_bufferLength, &tmpSize);
    printf("Buffer size is set successfully: %d\n", _bufferLength);

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

    printf("Connected to server.\n");

    while (1) {
        printf("Enter file name to download, send 'QUIT' to stop: ");

        // Send file name to server and receive file from server
        char sendBuffer[_bufferLength];
        char receiveBuffer[_bufferLength];
        fgets(sendBuffer, sizeof(sendBuffer), stdin);
        sendBuffer[strlen(sendBuffer) - 1] = '\0';

        if (strcmp(sendBuffer, _terminateChar) == 0) {
            break;
        }

        int sentMsgLen = strlen(sendBuffer) + 1;

        // Calculate time
        struct timespec startTime, finishTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

        write(serverSocket, sendBuffer, sentMsgLen);

        // Receive file size
        unsigned int fileSize = 0;
        unsigned int receivedSize = 0;
        read(serverSocket, &fileSize, sizeof(fileSize));
        // Handle error
        if (fileSize == 0) {
            printf("Cannot download file '%s'!\n", sendBuffer);
            continue;
        }
        // Receive file
        FILE *file = fopen(sendBuffer, "wb");
        while (receivedSize < fileSize) {
            int currRcvSize = read(serverSocket, receiveBuffer, _bufferLength);
            receivedSize += currRcvSize;
            fwrite(receiveBuffer, 1, currRcvSize, file);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &finishTime);
        long elapsedTime = (finishTime.tv_sec - startTime.tv_sec)*1e3 + (finishTime.tv_nsec - startTime.tv_nsec)/1e6;
        printf("Received file '%s' successfully after %ldms!\n", sendBuffer, elapsedTime);
        printf("Total received file size: %d bytes\n", receivedSize);
        fclose(file);
    }

    close(serverSocket);
    printf("Closed connection.\n");

    return 0;
}
