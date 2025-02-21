#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define TIMEOUT 1
#define BUFFER_SIZE 1024

int tcp_null_scan(char *target, int port) {
    int sock;
    struct sockaddr_in server;
    struct ip_header iphdr;
    struct tcp_header tcphd;
    struct pseudo_header psh;
    char packet[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);

    // Raw socket oluşturuluyor
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Zaman aşımı için sockopt ayarları yapılıyor
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT; // 1 saniye timeout
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, target, &server.sin_addr.s_addr) != 0) {
        perror("Invalid address/ Address not given");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // IP başlığı hazırlanıyor
    iphdr.ip_vhl = 0x45;
    iphdr.ip_tos = 0;
    iphdr.ip_len = sizeof(struct ip_header) + sizeof(struct tcp_header);
    iphdr.ip_id = htonl(54321);
    iphdr.ip_off = 0;
    iphdr.ip_ttl = 255;
    iphdr.ip_p = IPPROTO_TCP;
    iphdr.ip_src = inet_addr("127.0.0.1");
    iphdr.ip_dst = server.sin_addr.s_addr;
    iphdr.ip_sum = checksum(&iphdr, sizeof(struct ip_header));

    // TCP başlığı hazırlanıyor
    tcphd.source_port = 1538; // Totally random port
    tcphd.dest_port = htons(port);
    tcphd.sequence = 0;
    tcphd.acknowledgment = 0;
    tcphd.offset_reserve = 0x50;
    tcphd.flags = 0x00; // NULL SCAN (flags yok)
    tcphd.window = htons(5840);
    tcphd.urgent_pointer = 0;
    tcphd.checksum = 0;

    // Pseudo header hazırlanıyor
    psh.source_address = iphdr.ip_src;
    psh.dest_address = server.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcp_header));

    // Pseudo header ve TCP header'ı birleştirip checksum hesaplanıyor
    char pseudo_packet[sizeof(psh) + sizeof(tcphd)];
    memcpy(pseudo_packet, &psh, sizeof(psh));
    memcpy(pseudo_packet + sizeof(psh), &tcphd, sizeof(tcphd));
    tcphd.checksum = checksum(pseudo_packet, sizeof(pseudo_packet));

    // Paket hazırlanıyor
    memcpy(packet, &iphdr, sizeof(struct ip_header));
    memcpy(packet + sizeof(struct ip_header), &tcphd, sizeof(struct tcp_header));

    // TCP checksum tekrar hesaplanıyor
    tcphd.checksum = checksum(packet, sizeof(struct ip_header) + sizeof(struct tcp_header));

    // Paket gönderiliyor
    int result = sendto(sock, packet, sizeof(struct ip_header) + sizeof(struct tcp_header), 0,
                        (struct sockaddr *)&server, sizeof(server));
    if (result < 0) {
        perror("sendto failed");
        return -1;
    }
    printf("Null Scan packet sent to %s:%d\n", target, port);


	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);
    // Gelen yanıtı almak için recvfrom kullanılıyor
    int bytes_received = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr *)&from, &fromlen);
    if (bytes_received < 0) {
        perror("recvfrom failed");
        close(sock);
        return -1;
    }

    struct ip_header *iph = (struct ip_header *)response;
    struct tcp_header *tcph = (struct tcp_header *)(response + sizeof(struct ip_header));

    // Gelen paket TCP başlığını kontrol et
    if (tcph->flags == 0x14) {  // RST+ACK bayrağı (kapalı port)
        printf("Port %d CLOSED (RST received)\n", port);
    } else {
        // Eğer RST almadıysak, portun açık olduğuna karar verebiliriz.
        printf("Port %d OPEN (no RST received)\n", port);
    }

    close(sock);
    return 0;
}

