#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h> 

#define WHOIS_SERVER "whois.iana.org"
#define WHOIS_PORT 43
#define BUFFER_SIZE 1024

void whois(const char *domain) {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }


    struct hostent *host = gethostbyname(WHOIS_SERVER);
    if (host == NULL) {
        perror("Failed to resolve WHOIS server");
        close(socketfd);
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(WHOIS_PORT);
    memcpy(&server.sin_addr, host->h_addr, host->h_length);
	

    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) != 0) {
        perror("Connection failed");
        close(socketfd);
        exit(1);
    }


    char query[300];
    snprintf(query, sizeof(query), "%s\r\n", domain);
    if (send(socketfd, query, strlen(query), 0) < 0) {
        perror("Send failed");
        close(socketfd);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t recving;
    while ((recving = recv(socketfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[recving] = '\0';
        printf("%s", buffer);
    }

    if (recving < 0) {
        perror("Receiving failed");
    }

    close(socketfd);
}

int main() {
    char domain[256];
    printf("Enter domain: ");
    scanf("%255s", domain);
    whois(domain);
    return 0;
}

