//
// Created by ryuzaki on 2/19/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#define TIMEOUT 1
#define BUFFER_SIZE 1024

// SCTP Init Scan Fonksiyonu
int sctp_init_scan(char *target, int port) {
    // SCTP socket oluşturma
    int sock = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    if (sock < 0) {
        perror("SCTP socket creation failed..\n");
        return 0;
    }

    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    if (inet_pton(AF_INET, target, &serv.sin_addr.s_addr) != 1) {
        perror("Invalid IP address");
        close(sock);
        return 0;
    }

    struct sctp_initmsg initmsg;
    initmsg.sinit_num_ostreams = 1;
    initmsg.sinit_max_instreams = 1;
    initmsg.sinit_max_attempts = 1;

    // Set socket options
    if (setsockopt(sock, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg)) < 0) {
        perror("setsockopt failed");
        close(sock);
        return 0;
    }

    // Timeout işlemi
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // SCTP bağlantısını başlat
    int connectionStatus = connect(sock, (struct sockaddr *)&serv, sizeof(serv));
    if (connectionStatus < 0) {
        close(sock);
        return 0;  // Bağlantı başarısız, port kapalı
    }

    close(sock);
    return 1;  // Bağlantı başarılı, port açık
}

// SCTP Port Tarama
void scan_ports(char *targetIp) {
    int port;
    for (port = 1; port <= 1024; port++) {
        if (sctp_init_scan(targetIp, port)) {
            printf("SCTP Port %d is OPEN\n", port);
        } else {
            printf("SCTP Port %d is CLOSED\n", port);
        }
    }
}

int main() {
    char *targetIp = "127.0.0.1";  // Hedef IP
    scan_ports(targetIp);          // SCTP taramasını başlat
    return 0;
}
