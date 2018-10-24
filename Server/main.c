#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

void* receive(void* socket); //handler

void main() {
    int connection, result, length;
    WSADATA wsa;
    SOCKET clientSocket;
    char buff[10000];
    // setting address and port
    struct sockaddr_in server_address, cli;
    pthread_t sniffingThread;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    printf("\nInitialized...\n");
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET; // specify address and family
    server_address.sin_port = htons(80); // define port
    server_address.sin_addr.s_addr = INADDR_ANY;//inet_addr("10.15.65.7");

    result = bind(clientSocket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (result < 0) {
        printf("Error binding...\n");
        exit(1);
    }
    printf("successfully bond...\n");
     listen(clientSocket, 1);

    length = sizeof(cli); // get length of client
    connection = accept(clientSocket, (struct sockaddr *)&cli, &length);
    if (connection < 0) {
        printf("Error connecting client");
        exit(1);
    }
    printf("Connection accepted...\n");
    memset(buff, 0, 10000);
    printf("Enter your messages one by one and press return key!\n");
    result = pthread_create(&sniffingThread, NULL, receive, (void *) connection);

    if (result) {
        printf("Error creating thread...\n");
        exit(1);
    }

    while (fgets(buff, 10000, stdin) != NULL) {
        result = sendto(connection, buff, 10000, 0, (struct sockaddr *) &cli, length);
        if (strncmp("quit", buff, 4) == 0) {
            printf("Exiting chat...\n");
            break;
        }
        //printf("Server: ");

        if (result < 0) {
            printf("Error sending data...\n");
            exit(1);
        }
    }
    pthread_exit(NULL);
}
void *receive(void* socket) { //handler
    char buff[10000];
    int result, sock;
    sock = (int) socket;
    memset(buff, 0, 10000);
    while(1) {

        if (strncmp("quit", buff, 4) == 0) {
            printf("Exiting chat...\n");
            break;
        }
        result = recvfrom(sock, buff, 10000, 0, NULL, NULL);
        if (result < 0) {
            printf("Error receiving the message\n");
        } else {
            printf("client: ");
            fputs(buff, stdout);
            printf("\n");
        }
    }
}
