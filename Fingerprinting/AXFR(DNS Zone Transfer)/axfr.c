#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 512

// DNS Header
struct DNSHeader {
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

// DNS Question Section
struct DNSQuestion {
    unsigned short qtype;
    unsigned short qclass;
};

// DNS Message Constants
#define DNS_PORT 53
#define DNS_AXFR 252  // AXFR Record Type
#define MAX_RETRIES 3

// Helper function to send a DNS query
void sendAXFRQuery(int sockfd, struct sockaddr_in server_addr, const char *domain) {
    char buffer[BUFFER_SIZE];
    struct DNSHeader *header = (struct DNSHeader *)buffer;
    struct DNSQuestion *question = (struct DNSQuestion *)(buffer + sizeof(struct DNSHeader));

    memset(buffer, 0, BUFFER_SIZE);

    // Set DNS header
    header->id = htons(12345);  // Transaction ID
    header->flags = htons(0x0100);  // Standard query
    header->qdcount = htons(1);  // Number of questions

    // Set DNS question
    strcpy(buffer + sizeof(struct DNSHeader), domain);  // Domain name
    question->qtype = htons(DNS_AXFR);  // AXFR type
    question->qclass = htons(1);  // IN class (internet)

    // Send the query
    ssize_t sent_bytes = sendto(sockfd, buffer, sizeof(struct DNSHeader) + strlen(domain) + 1 + sizeof(struct DNSQuestion), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0) {
        perror("Failed to send AXFR query");
        exit(1);
    }
}

// Function to handle the AXFR response
void receiveAXFRResponse(int sockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t recv_bytes;

    while (1) {
        recv_bytes = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (recv_bytes < 0) {
            perror("Failed to receive AXFR response");
            exit(1);
        }

        // Print the received data (for now, just raw bytes)
        printf("Received %zd bytes\n", recv_bytes);
        for (int i = 0; i < recv_bytes; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");

        // The AXFR transfer will stop when all records are sent.
        if (recv_bytes < BUFFER_SIZE) {
            break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define the DNS server address (e.g., Google's DNS)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DNS_PORT);
    server_addr.sin_addr.s_addr = inet_addr("8.8.8.8");  // Google's public DNS

    const char *domain = "example.com";  // Domain to query

    // Send the AXFR query
    sendAXFRQuery(sockfd, server_addr, domain);

    // Receive the AXFR response
    receiveAXFRResponse(sockfd);

    // Close the socket
    close(sockfd);

    return 0;
}

