#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define TIMEOUT 1
#define BUFFER_SIZE 1024

int common_ports[9] = {22, 80, 443, 21, 25, 53, 110, 143, 3389};

int tcp_syn_scan(char *ip, int port) {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        perror("Socket creation failed");
        return 0;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server.sin_addr.s_addr) != 1) {
        perror("Invalid IP address");
        close(socketfd);
        return 0;
    }

    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int connectionStatus = connect(socketfd, (struct sockaddr *)&server, sizeof(server));

    if (connectionStatus == -1) {
        close(socketfd);
        return 0;
    } else {
        close(socketfd);
        return 1;
    }
}

void scan_ports(char *ip) {
    int port;
    for (port = 1; port < 1024; port++) {
            if (tcp_syn_scan(ip, port)) {
                printf("Port %d OPEN \n", port);
            }else {
                printf("Port %d CLOSED \n", port);
            }


    }
}

