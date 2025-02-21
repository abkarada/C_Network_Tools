#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

typedef struct pseudo_header{
u_int32_t src;
u_int32_t dst;
u_int8_t placeholder;
u_int8_t p;
u_int16_t len;
}pshd;

typedef struct tcp_header{
u_int16_t src_port;
u_int16_t dst_port;
u_int32_t seq;
u_int32_t ack;
u_int8_t off;
u_int8_t rsrv;
u_int8_t flags;
u_int16_t window;
u_int16_t sum;
u_int16_t urg_pointer;
}tcphd;


typedef struct ip_header{
u_int8_t ip_vhl;
u_int8_t ip_tos;
u_int16_t ip_len;
u_int16_t ip_id;
u_int16_t off;
u_int8_t ip_ttl;
u_int8_t ip_p;
u_int16_t ip_sum;
u_int32_t ip_src;
u_int32_t ip_dst;
}iphd;

unsigned short(void *b, int len){
unsigned short *buf = b;//16
unsigned int sum = 0;//32
unsigned short result;//16
	
	for(sum = 0; len > 1; len -= 2){
		sum += *buf++;
	
	}
	if(len){
		sum += (unsigned char *)buf;
	
	}

	sum = (sum >> 16) + (sum &0xFFFF);
	sum += (sum >> 16);

	result = ~sum;

	return result;
}

int tcp_ack_scan(char *targer, int port){
	struct sockaddr_in server;
	iphd ip;
	tcphd tcp;
	pshd ps;
	char buffer[BUFFER_SIZE];
	char response[BUFFER_SIZE];
	int raw_socket;


raw_socket = socket(AF_INET, RAW_SOCK, IPROTO_RAW);

server.sin_family = AF_INET;
server.sin_port = port;
int address_val = inet_pton(AF_INET, ip, &server.sin_addr.s_addr);
if(address_val != 1){perror("Invalid Address..\n");exit(EXIT_FAILURE);}



tcp.src_port = 54321//Source yani kaynak bizim ileteceğimiz port random olabilir
tcp.dest_port = port;//Bu port durumunu bilmek istediğimiz port 
tcp.seq = 0;
tcp.ack = 0;
tcp.off = 0x50;//Standar offset
tcp.flags = 0x10; // ACK = 0x10 
tcp.window = htons(5840);//Linux pencere boyutu 
tcp.urg_pointer = 0;
tcp.checksum = 0;



ip.ip_vhl = 0x45;
ip.ip_tos = 0;
ip.ip_len = sizeof(iphd) + sizeof(tcphd);//Burada tcp'nin IP headerına
					 //GÖMÜLDÜĞÜNÜ anlamamız gerekiyor. 
ip.ip_id = htonl(54321);
ip.ip_off = 0;
ip.ip_ttl = 255;
ip.ip_prot = IPROTO_TCP;
ip.ip_src = inet_addr("127.0.0.1");
ip.ip_dest = server.sin_addr.s_addr;
ip.ip_checksum = checksum(&ip, sizeof(iphd));

ps.src = ip.ip_src;
ps.dst = server.sin_addr.s_addr;
ps.placeholder = 0;
ps.p = IPROTO_TCP;
ps.len = htons(sizeof(tcphd));

char pseudo_packet[sizeof(pshd) + sizeof(tcphd)];
memcpy(pseudo_packet, &ps,sizeof(pshd));
memcpy(pseudo_packet + sizeof(pshd), &tcp, sizeof(tcphd));//--->Pseudo header ın içine TCP header ı gömdük.
tcp.checksum = checksum(pseudo_packet, sizeof(pseudo_packet));//Geçici bir hesaplama anormali tespiti için gerek.

char packet[sizeof(iphd)+sizeof(tcphd)];//Pseudo Header gerçek pakete eklenmez.
memcpy(packet, &ip, sizeof(iphd));
memcpy(packet + sizeof(iphd), &tcp, sizeof(tcphd));


tcp.checksum = checksum(packet, sizeof(packet));//Yukarıda yaptığımız tcp checksum hesabı hiçe sayılıp güncellendi

//Oluşturduğumuz paketi göndereceğiz normal bir tcp protokolündeki gibi send() i kullanamayız raw soketlerle ve udp protokolleri//yle çalışırken her zaman sendto() fonksiyonunu kullanacağız 

int sending_test = sendto(raw_socket, packet, sizeof(packet), 0 , (struct sockaddr *)server, sizeof(server));
	if(sending_test < 0){perror("Send failed...\n");exit(EXIT_FAILURE);}

	struct timeval tv;
	tv.tv_sec = TIMEOUT;
	tv.tv_usec = 0;
	int time_exceed_test = setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	time_exceed_test < 0 ? perror("RCVTIMEO errror..\n"): ((void)0);

	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

recvfrom(raw_socket, response, sizeof(respones), 0, (struct sockaddr *)from, &fromlen) < 0 ? perror("packet lost..\n") : ((void)0);


iphd *response_ip = (iphd *)response;
tcphd *response_tcp = (tcphd *)(response + sizeof(iphd));

 response_tcp == 0x04 ? return 1: return 0;

 close(raw_socket);

}


void scan_ports(char *ip) {
    int port;
    for (port = 1; port < 1024; port++) {
        if (tcp_ack_scan(ip, port)) {
            printf("Port %d OPEN \n", port);
        }else {
            printf("Port %d CLOSED \n", port);
        }


    }
}

