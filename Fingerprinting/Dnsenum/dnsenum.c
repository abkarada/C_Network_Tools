#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DNS_PORT 53
#define BUFFER_SIZE 512

// DNS Header Yapısı
struct DNS_HEADER {
    unsigned short id;       // Kimlik
    unsigned char rd :1;     // Tekrar çözümleme
    unsigned char tc :1;     // İletme
    unsigned char aa :1;     // Yetkili cevap
    unsigned char opcode :4; // Sorgu tipi
    unsigned char qr :1;     // Sorgu / Cevap
    unsigned char rcode :4;  // Yanıt kodu
    unsigned char cd :1;     // Kontrollü çözümleme
    unsigned char ad :1;     // Doğrulama
    unsigned char z :1;      // Gelecek için ayrılmış
    unsigned char ra :1;     // Tekrar çözümleme
    unsigned short qdcount;  // Sorgu sayısı
    unsigned short ancount;  // Yanıt sayısı
    unsigned short nscount;  // Yetkili kayıtlar
    unsigned short arcount;  // Ek kayıtlar
};

// DNS Sorgu Türleri
#define TYPE_A     1   // IPv4 adresi
#define TYPE_NS    2   // Ad sunucusu
#define TYPE_CNAME 5   // Alias
#define TYPE_SOA   6   // Yetkili bilgi
#define TYPE_MX    15  // Mail değişimi
#define TYPE_TXT   16  // Metin kaydı

// Sorgu sınıfı (İnternet için: 1)
#define CLASS_IN 1

void encode_domain_name(char *dest, const char *domain) {
    char *dot = strchr(domain, '.');
    while (dot) {
        *dest++ = dot - domain;
        strncpy(dest, domain, dot - domain);
        dest += dot - domain;
        domain = dot + 1;
        dot = strchr(domain, '.');
    }
    *dest++ = strlen(domain);
    strcpy(dest, domain);
    dest += strlen(domain);
    *dest = 0;
}

void send_dns_query(const char *dns_server, const char *domain, int query_type) {
    int sock;
    struct sockaddr_in server_addr;
    unsigned char buffer[BUFFER_SIZE];

    // DNS Header ayarları
    struct DNS_HEADER *dns = (struct DNS_HEADER *)&buffer;
    dns->id = htons(getpid());
    dns->qr = 0; // Sorgu
    dns->opcode = 0; // Standart sorgu
    dns->aa = 0;
    dns->tc = 0;
    dns->rd = 1; // Tekrar çözümleme
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->qdcount = htons(1); // 1 sorgu
    dns->ancount = 0;
    dns->nscount = 0;
    dns->arcount = 0;

    // Alan adı ekleme
    unsigned char *qname = buffer + sizeof(struct DNS_HEADER);
    encode_domain_name(qname, domain);
    
    // Sorgu tipi ve sınıf ekleme
    unsigned short *qtype = (unsigned short *)(qname + strlen(qname) + 1);
    *qtype = htons(query_type);
    unsigned short *qclass = (unsigned short *)(qtype + 1);
    *qclass = htons(CLASS_IN);

    // Soket oluştur
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket oluşturulamadı");
        return;
    }

    // Sunucu adresi
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DNS_PORT);
    server_addr.sin_addr.s_addr = inet_addr(dns_server);

    // Sorguyu gönder
    int query_size = (int)(qclass + 1) - (int)buffer;
    if (sendto(sock, buffer, query_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Sorgu gönderilemedi");
        close(sock);
        return;
    }

    // Yanıt al
    socklen_t server_len = sizeof(server_addr);
    int response_size = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_len);
    if (response_size < 0) {
        perror("Yanıt alınamadı");
        close(sock);
        return;
    }

    printf("Received DNS response:\n");
    for (int i = 0; i < response_size; i++) {
        printf("%02x ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    close(sock);
}

int main() {
    char domain[256];
    char dns_server[256] = "8.8.8.8"; // Google DNS
    int query_type;

    printf("DNS Enumeration Tool (C ile)\n");
    printf("Hedef Domain: ");
    scanf("%s", domain);

    printf("Sorgu Türü:\n");
    printf("1. A (IPv4 Adresi)\n2. NS (Name Server)\n3. MX (Mail Exchange)\n4. CNAME (Alias)\n5. SOA (Start of Authority)\n6. TXT (Text Record)\nSeçiminiz: ");
    scanf("%d", &query_type);

    switch (query_type) {
        case 1: query_type = TYPE_A; break;
        case 2: query_type = TYPE_NS; break;
        case 3: query_type = TYPE_MX; break;
        case 4: query_type = TYPE_CNAME; break;
        case 5: query_type = TYPE_SOA; break;
        case 6: query_type = TYPE_TXT; break;
        default: printf("Geçersiz seçim!\n"); return 1;
    }

    send_dns_query(dns_server, domain, query_type);

    return 0;
}
