//
// Created by ryuzaki on 2/19/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#define TIMEOUT 1
#define BUFFER_SIZE 1024

int udp_scan(char *target, int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {perror("socket creation failed..\n"); exit(1);}

    struct  sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    if (inet_pton(AF_INET, target, &serv.sin_addr.s_addr) != 1) {
        perror("Invalid IP address");
        exit(1);
    }
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[BUFFER_SIZE];

    int icmp_message = sendto(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv, sizeof(serv));
    if (icmp_message == -1) {perror("Could not sent it"); exit(1);}
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv, sizeof(serv));
    if (n < 0) {
    return   1;
    }else {
        return 0;
    }

}

// 1024 portu tarama fonksiyonu
void scan_ports(char *targetIp) {
    int port;
    for (port = 1; port <= 1024; port++) {
        if (udp_scan(targetIp, port)) {
            printf("UDP Port %d is OPEN\n", port);
        } else {
            printf("UDP Port %d is CLOSED\n", port);
        }
    }
}