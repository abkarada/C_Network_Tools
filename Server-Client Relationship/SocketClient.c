#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int socketfd;  // Global değişken olarak tanımlanmalı

void *receiveMessages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t amountReceived = recv(socketfd, buffer, BUFFER_SIZE - 1, 0);
        if (amountReceived <= 0) {
            if (amountReceived == 0) {
                printf("Server disconnected.\n");
            } else {
                perror("recv failed");
            }
            close(socketfd);
            exit(0);
        }
        buffer[amountReceived] = '\0';
        printf("Message from other client: %s\n", buffer);
    }
    return NULL;
}

int main() {
    socketfd = socket(AF_INET, SOCK_STREAM, 0);  // Tekrar tanımlamıyoruz
    if (socketfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    char *ip = "127.0.0.1";

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(55555);
    
    if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    int result = connect(socketfd, (struct sockaddr *)&address, sizeof(address));
    if (result < 0) {
        perror("Connection failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");
    
    printf("Enter your nickname: ");

char nickname[50];
fgets(nickname, sizeof(nickname), stdin);
nickname[strcspn(nickname, "\n")] = '\0'; // \n karakterini kaldır
send(socketfd, nickname, strlen(nickname), 0);

    
    
        pthread_t recvThread;
    if (pthread_create(&recvThread, NULL, receiveMessages, NULL) != 0) {
        perror("Failed to create thread");
        close(socketfd);
        exit(EXIT_FAILURE);
    }
    pthread_detach(recvThread);

    char *line = NULL;
    size_t lineSize = 0;
    printf("Type to send (type 'exit' to quit).....\n");

    while (true) {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        if (charCount <= 0) break;

        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        ssize_t sent = send(socketfd, line, strlen(line), 0);
        if (sent < 0) {
            perror("send failed");
            break;
        }
    }

    free(line);
    close(socketfd);
    return 0;
}

