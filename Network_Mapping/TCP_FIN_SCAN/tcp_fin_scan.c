//
// Created by ryuzaki on 2/19/25.
//
//
// Created by ryuzaki on 2/19/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define TIMEOUT 1
#define BUFFER_SIZE 1024


#define FIN 0x01
#define SYN 0x02
#define RST 0x04    
#define PSH 0x08
#define ACK 0x10
#define URG 0x20

// Pseudo header için gerekli yapı
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

// TCP başlık
struct tcp_header {
    u_int16_t source_port;
    u_int16_t dest_port;
    u_int32_t sequence;
    u_int32_t acknowledgment;
    u_int8_t offset_reserve;
    u_int8_t flags;
    u_int16_t window;
    u_int16_t checksum;
    u_int16_t urgent_pointer;
};

// IP başlık
struct ip_header {
    u_int8_t ip_vhl; // version + header length
    u_int8_t ip_tos; // type of service
    u_int16_t ip_len; // total length
    u_int16_t ip_id; // identification
    u_int16_t ip_off; // fragment offset field
    u_int8_t ip_ttl; // time to live
    u_int8_t ip_p; // protocol
    u_int16_t ip_sum; // checksum
    u_int32_t ip_src, ip_dst; // source and destination addresses
};

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *) buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int tcp_fin_scan(char *target, int port) {
int sock;
    struct sockaddr_in server;
    struct ip_header iphdr;
    struct tcp_header tcphd;
    struct pseudo_header psh;

    char packet[BUFFER_SIZE];
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, target, &server.sin_addr.s_addr) !=0) {
        perror("Invalid address/ Address not given");
        exit(EXIT_FAILURE);
    }

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

    tcphd.source_port = 1538;//Totally random port
    tcphd.dest_port = htons(port);
    tcphd.sequence = 0;
    tcphd.acknowledgment = 0;
    tcphd.offset_reserve = 0x50;
    tcphd.flags = 0x01;//FIN
    tcphd.window = htons(5840);
    tcphd.urgent_pointer = 0;
    tcphd.checksum = 0;


    psh.source_address = iphdr.ip_src;
    psh.dest_address = server.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcp_header));


    memcpy(packet, &iphdr, sizeof(struct ip_header));
    memcpy(packet + sizeof(struct ip_header), &tcphd, sizeof(struct tcp_header));

    tcphd.checksum = checksum(packet, sizeof(struct ip_header) + sizeof(struct tcp_header));

    int result = sendto(sock, packet, sizeof(struct ip_header) + sizeof(struct tcp_header), 0,
                        (struct sockaddr *)&server, sizeof(server));
    if (result < 0) {
        perror("sendto failed");
    }
    printf("FIN Scan packet sent to %s:%d\n", target, port);


	struct timeval timeout;
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
	int time_exceed_test = setsockopt(sock, SOL_SOCKET,SO_RCVTIMEO,&timeout, sizeof(timeof)); 

	if(time_exceed_test < 0){
		perror("Recv error time out..\n");
		exit(EXITFAILURE);
	}
	
	
		char response[BUFFER_SIZE];
		struct sockaddr_in from;
		socklen_t fromlen = sizeof(from);
		
		int response_check = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*)from, &fromlen);
		if(response_check < 0){perror("Packet Loss...\n");exit(EXIT_FAILURE);}
		
		struct ip_header *response_ip = (struct ip_header *)response;
		struct tcp_header *response_tcp = (struct tcp_header *)(response + sizeof(ip_header));
		
		if(response_tcp == 0x04){return 1;}
		return 0;
		
		
		
		
		


}

void scan_ports(char *ip) {
    int port;
    for (port = 1; port < 1024; port++) {
        if (tcp_fin_scan(ip, port)) {
            printf("Port %d OPEN \n", port);
        }else {
            printf("Port %d CLOSED \n", port);
        }


    }
}


