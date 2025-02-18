#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 512
#define DNS_PORT 53

// DNS Header Yapısı (12 Byte)
struct DNS_HEADER {
    unsigned short id;       // Sorgu ID'si
    unsigned short flags;
    unsigned short qd_count; // Sorgu sayısı
    unsigned short an_count; // Yanıt sayısı
    unsigned short ns_count; // Yetkili sunucu sayısı
    unsigned short ar_count; // Ek kayıt sayısı
};

// Sorgu Tipleri (QTYPE)
#define A_RECORD 1  // IPv4 Adresi

// Sorgu Sınıfı (QCLASS)
#define IN_CLASS 1  // Internet Sınıfı

// Alan adını DNS formatına çevir (örneğin "google.com" → "\x06google\x03com\x00")
void encode_domain_name(unsigned char *dns_format, const char *host) {
    int lock = 0;
    strcat((char *)dns_format, ".");

    for (int i = 0; i < strlen(host); i++) {
        if (host[i] == '.') {
            dns_format[lock] = i - lock;
            lock = i + 1;
        } else {
            dns_format[i + 1] = host[i];
        }
    }
}

// DNS Sorgusu Oluştur
int create_dns_query(unsigned char *buffer, const char *host) {
    struct DNS_HEADER *dns = (struct DNS_HEADER *) buffer;
    dns->id = htons(1234);   // Rastgele ID
    dns->flags = htons(0x0100); // Standart sorgu (QR=0, Opcode=0, RD=1)
    dns->qd_count = htons(1);   // 1 Sorgu
    dns->an_count = 0;
    dns->ns_count = 0;
    dns->ar_count = 0;

    // Sorgu kısmı (Alan adını ekle)
    unsigned char *query = buffer + sizeof(struct DNS_HEADER);
    encode_domain_name(query, host);

    // QTYPE ve QCLASS ekle
    int query_length = strlen((char *)query) + 1;
    unsigned short *qtype = (unsigned short *)(query + query_length);
    *qtype = htons(A_RECORD);
    unsigned short *qclass = (unsigned short *)(query + query_length + 2);
    *qclass = htons(IN_CLASS);

    return sizeof(struct DNS_HEADER) + query_length + 4;
}

// DNS Sorgusu Gönder
void send_dns_query(const char *dns_server, const char *host) {
    struct sockaddr_in server;
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0) {
        perror("Socket oluşturulamadı");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(DNS_PORT);
    inet_pton(AF_INET, dns_server, &server.sin_addr);

    unsigned char buffer[BUFFER_SIZE];
    int query_length = create_dns_query(buffer, host);

    // Sorguyu DNS sunucusuna gönder
    sendto(sock, buffer, query_length, 0, (struct sockaddr *)&server, sizeof(server));

    // Yanıtı al
    socklen_t server_len = sizeof(server);
    int response_length = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server, &server_len);

    if (response_length < 0) {
        perror("Yanıt alınamadı");
        close(sock);
        return;
    }

    // Yanıtı ayrıştır
    parse_dns_response(buffer, response_length);

    close(sock);
}

// Yanıtı Çözümle
void parse_dns_response(unsigned char *buffer, int length) {
    struct DNS_HEADER *dns = (struct DNS_HEADER *) buffer;
    unsigned char *reader = buffer + sizeof(struct DNS_HEADER);

    printf("\n--- DNS Yanıtı ---\n");
    printf("Yanıt Sayısı: %d\n", ntohs(dns->an_count));

    // Question Section'ı atla
    while (*reader != 0) reader++;
    reader += 5; // null bit + QTYPE (2) + QCLASS (2)

    // Yanıt Bölümünü Ayrıştır
    for (int i = 0; i < ntohs(dns->an_count); i++) {
        reader += 2; // NAME alanı (sıkıştırılmış olabilir)
        unsigned short type, class;
        memcpy(&type, reader, 2);
        memcpy(&class, reader + 2, 2);
        reader += 8; // TYPE, CLASS, TTL (4 byte)

        unsigned short data_length;
        memcpy(&data_length, reader, 2);
        reader += 2;

        if (ntohs(type) == A_RECORD) { // Eğer IPv4 adresi ise
            struct in_addr ip;
            memcpy(&ip, reader, 4);
            printf("IP Adresi: %s\n", inet_ntoa(ip));
        }
        reader += ntohs(data_length);
    }
}

// Ana Fonksiyon
int main() {
    const char *dns_server = "8.8.8.8"; // Google DNS
    const char *host = "example.com";

    printf("DNS Sorgusu Gönderiliyor: %s\n", host);
    send_dns_query(dns_server, host);

    return 0;
}

