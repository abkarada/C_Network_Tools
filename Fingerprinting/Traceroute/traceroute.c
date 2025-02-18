#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#define MAX_TTL 30
#define TIMEOUT 1  // 1 saniye
#define DEST_PORT 33434 // Traceroute default UDP portu

// Pseudo header structure for checksum calculation
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

// Calculate checksum
unsigned short checksum(void *b, int len) {    
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Create UDP packet
int create_packet(char *packet, int ttl) {
    struct iphdr *iph = (struct iphdr *) packet;
    struct udphdr *udph = (struct udphdr *) (packet + sizeof(struct iphdr));
    struct pseudo_header psh;
    
    memset(packet, 0, 4096);
    
    // IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    iph->id = htonl(54321); // random id
    iph->frag_off = 0;
    iph->ttl = ttl;
    iph->protocol = IPPROTO_UDP;
    iph->saddr = inet_addr("1.2.3.4");  // Source IP (can be any)
    iph->daddr = inet_addr("8.8.8.8");  // Destination IP
    
    iph->check = checksum((unsigned short *)packet, iph->tot_len);
    
    // UDP Header
    udph->source = htons(33434); 
    udph->dest = htons(DEST_PORT);
    udph->len = htons(sizeof(struct udphdr));
    udph->check = 0;  // Leave checksum 0 now, filled by pseudo header

    // Now the pseudo header
    psh.source_address = inet_addr("1.2.3.4");
    psh.dest_address = inet_addr("8.8.8.8");
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr);
    char *pseudogram = malloc(psize);
    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr));
    
    udph->check = checksum((unsigned short *)pseudogram, psize);
    free(pseudogram);
    
    return sizeof(struct iphdr) + sizeof(struct udphdr);
}

// Perform a traceroute
void traceroute(const char *hostname) {
    struct sockaddr_in dest_addr;
    struct timeval tv;
    int sockfd, ttl, n;
    char packet[4096];
    struct iphdr *iph = (struct iphdr *)packet;
    struct sockaddr_in r_addr;
    socklen_t len = sizeof(r_addr);
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        fprintf(stderr, "Host not found\n");
        return;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr = *((struct in_addr *)host->h_addr);
    
    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket error");
        return;
    }

    for (ttl = 1; ttl <= MAX_TTL; ttl++) {
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        // Create packet
        int packet_size = create_packet(packet, ttl);
        
        // Send packet
        n = sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (n < 0) {
            perror("Send failed");
            return;
        }

        // Receive response
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        n = recvfrom(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&r_addr, &len);
        if (n < 0) {
            printf("TTL %d: Request timed out.\n", ttl);
        } else {
            printf("TTL %d: %s\n", ttl, inet_ntoa(r_addr.sin_addr));
            if (r_addr.sin_addr.s_addr == dest_addr.sin_addr.s_addr) {
                break;  // Reached destination
            }
        }
    }
    close(sockfd);
}

int main() {
    char domain[256];
    printf("Enter the hostname to perform a traceroute: ");
    scanf("%s", domain);
    traceroute(domain);
    return 0;
}

