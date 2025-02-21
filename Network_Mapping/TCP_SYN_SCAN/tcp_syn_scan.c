#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#define TIMEOUT 1
#define BUFFER_SIZE 1024


int syn_scan(char *targetIp, int port) {
    //Create a socket
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd == -1) { perror("socket creation failed"); exit(1);}

    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
//Create a  *sockaddr_in address
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET , targetIp, &server.sin_addr.s_addr) != 1) {
    perror("Invalid IP address");
    }
    //Send SYN message by using Datagram trick
    int syn_message = sendto(sockfd, (struct sockaddr*)&server, sizeof(server), 0,);
    if (syn_message == -1) { perror("Failed to send SYN"); exit(1);}

    char buffer[BUFFER_SIZE];
    int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (n > 0) {
        return 1; // Port açık
    } else {
        return 0; // Port kapalı
    }
}



}

void scan_ports(char *targetIp) {
    int port;
    for (port = 1; port < 1024; port++) {
        if (syn_scan(targetIp, port)) {
            printf("Port %d is OPEN \n", port);
        }else {
            printf("Port %d is CLOSED \n", port);


        }

    }

}