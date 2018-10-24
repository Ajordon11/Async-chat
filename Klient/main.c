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

void *receive(void* socket);

int main() {
    int connection, result;
    WSADATA wsa;
    SOCKET clientSocket;
    char buff[10000];
    // setting address and port
    struct sockaddr_in server_address;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0 );
    pthread_t sniffingThread;
    if (clientSocket == INVALID_SOCKET) {
        printf("Error with creating socket\n");
        return 1;
    }
    printf("\nInitialized...\n");


    memset(&server_address, 0, sizeof(server_address)); // NULL address
    server_address.sin_family = AF_INET; // specify address and family
    server_address.sin_port = htons(80); // define port
    server_address.sin_addr.s_addr = inet_addr("147.175.123.218");

    connection = connect(clientSocket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection < 0) {
        printf("Error connecting to the server...\n");
        exit(1);
    }

    printf("Connected.\n");
    memset(buff, 0, 10000);
    printf("Enter your messages one by one and press return key!\n");

    result = pthread_create(&sniffingThread, NULL, receive, (void *) clientSocket);
    if (result) {
        printf("Error creating a thread...\n");
        exit(1);
    }

    while (fgets(buff, 10000, stdin) != NULL) {
        result = sendto(clientSocket, buff, 10000, 0, (struct sockaddr *) &server_address, sizeof(server_address));
        if (strncmp("quit", buff, 4) == 0) {
            printf("Exiting chat...\n");
            break;
        }
        if (result < 0) {
                printf("Error sending data...\n");
            exit(1);
        }
    }

    return 0;
}

void *receive(void* socket) { //handler
    char buff[10000];
    int result, sock;
    sock = (int) socket;
    memset(buff, 0, 10000);
    while(1) {
        result = recvfrom(sock, buff, 10000, 0, NULL, NULL);
        if (strncmp("quit", buff, 4) == 0) {
            printf("Connection lost, write 'quit' for exit...\n");
            break;
        }

        if (result < 0) {
                printf("Error receiving the message\n");
        }
        else {

            printf("server: ");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            fputs(buff, stdout);
            printf("\n");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        }
    }

}
