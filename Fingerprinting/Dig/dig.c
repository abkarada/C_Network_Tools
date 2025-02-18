#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DNS_SERVER "8.8.8.8"
#define DNS_PORT 53

// DNS header structure
typedef struct {
    unsigned short id;
    unsigned short flags;
    unsigned short questions;
    unsigned short answerRRs;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
} DNS_HEADER;

// DNS question section
typedef struct {
    unsigned short qtype;
    unsigned short qclass;
} DNS_QUESTION;

// DNS response section for A record
typedef struct {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
} DNS_RR;

void build_query(char *domain, unsigned char *buf, int *query_len) {
    DNS_HEADER *dns_header = (DNS_HEADER *)buf;
    dns_header->id = htons(12345);
    dns_header->flags = htons(0x0100);  // Standard query
    dns_header->questions = htons(1);   // Only 1 question
    dns_header->answerRRs = 0;
    dns_header->authorityRRs = 0;
    dns_header->additionalRRs = 0;

    // DNS Question Section
    unsigned char *qname = buf + sizeof(DNS_HEADER);
    int i, j = 0;
    for(i = 0; i < strlen(domain); i++) {
        if(domain[i] == '.') {
            qname[j] = i - j;
            j = i + 1;
        }
    }
    qname[j] = strlen(domain) - j;

    // Query type and class
    DNS_QUESTION *dns_question = (DNS_QUESTION *)(qname + strlen(domain) + 1);
    dns_question->qtype = htons(1);  // A record (IPv4 address)
    dns_question->qclass = htons(1); // IN (Internet)
    
    *query_len = sizeof(DNS_HEADER) + strlen(domain) + 2 + sizeof(DNS_QUESTION);
}

void parse_response(unsigned char *buf, int response_len) {
    DNS_RR *dns_rr = (DNS_RR *)(buf + sizeof(DNS_HEADER) + strlen((char *)buf + sizeof(DNS_HEADER)) + 2 + sizeof(DNS_QUESTION));
    
    if (dns_rr->type == htons(1)) {  // A record
        unsigned char *ip = buf + sizeof(DNS_HEADER) + strlen((char *)buf + sizeof(DNS_HEADER)) + 2 + sizeof(DNS_QUESTION) + sizeof(DNS_RR);
        printf("IP Address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    }
}

int main() {
    int sock;
    struct sockaddr_in dest;
    unsigned char buffer[512];

    char domain[] = "www.example.com";
    int query_len;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset((char *)&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DNS_PORT);
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER);

    build_query(domain, buffer, &query_len);

    if (sendto(sock, buffer, query_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
        return 1;
    }

    int dest_len = sizeof(dest);
    int response_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&dest, &dest_len);

    if (response_len < 0) {
        perror("Recv failed");
        return 1;
    }

    parse_response(buffer, response_len);
    close(sock);
    return 0;
}
