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

typedef struct pseuo_header {
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t placeholder
	u_int8_t protocol;
	u_int16_t len;
}pshd;


typedef struct ıp_header{
	u_int8_t ip_vhl;// version + header lenght
	u_int8_t ip_tos;
	u_int16_t ip_len;
	u_int16_t ip_id;
	u_int16_t ip_off;
	u_int8_t ip_ttl;
	u_int8_t ip_prot;
	u_int16_t ip_sum;
	u_int32_t ip_src;
	u_int32_t ip_dst;
}iphd;


struct tcp_header {
	u_int16_t src_port;
	u_int16_t dst_port;
	u_int32_t seq;
	u_int32_t ack;
	u_int8_t off;
	u_int8_t rsrv;
	u_int8_t flags;
	u_int16_t window;
	u_int16_t checksum;
	u_int16_t urg_pointer;
}tcphd;


unsigned short checksum(void *b, int len){
unsigned short *buff = b;
unsigned int sum = 0 ;
unsigned short result;

for(sum = 0;len > 1;len -=2){
	sum += *buff++;

if(len)
	sum += *(unsigned char *)buff;

sum = (sum >> 16) + (sum &0XFFFF);
sum += (sum >> 16);

result = ~sum;

return result;
}

int tcp_xmas_scan(char *target, int port){
	pshd ps;
	iphd ip;
	tcphd tcp;

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = port;
	if(inet_pton(AF_INET, target, &server.sin_addr.s_addr) != 1){
		perror("Invalid Ip Address...\n");
		exit(EXIT_FAILURE);
	}

int r_socket = socket(AF_INET, SOCK_RAW, IPROTO_RAW);
if(r_socket < 0){
	perror("Socket Failure...\n");
	exit(EXIT_FAILURE);
} 

//Kapsülleme aşağı yönde bu yüzden ip tcp yi de içerecek başka bir deyişle:
//tcp Ip nin üzerine oturulacak bu yüzden ilk tcp başlığının yapısını
//oluşturmalıyım

tcp.src_port = 54321//Source yani kaynak bizim ileteceğimiz port random olabilir
tcp.dest_port = port;//Bu port durumunu bilmek istediğimiz port 
tcp.seq = 0;
tcp.ack = 0;
tcp.off = 0x50;//Standar offset
tcp.flags = 0x29; // FIN PSH URG = 0x29 
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

ps.source_address = ip.ip_src;
ps.destination_address = server.sin_addr.s_addr;
ps.placeholder = 0;
ps.protocol = IPROTO_TCP;
ps.len = htons(sizeof(tcphd));

char pseudo_packet[sizeof(pshd) + sizeof(tcphd)];
memcpy(pseudo_packet, &ps, sizeof(pshd));
memcpy(pseudo_packet + sizeof(pshd), &tcp, sizeof(tcphd));
tcp.checksum = checksum(pseudo_packet, sizeof(pseudo_packet));





char packet[BUFFER_SIZE];

memcpy(packet, &ip, sizeof(iphd));
memcpy(packet + sizeof(iphd), &tcp, sizeof(tcphd));

tcp.checksum = checksum(packet, sizeof(iphd) + sizeof(tcphd));

int result = sendto(r_socket, packet, sizeof(iphd) + sizeof(tcphd), 0,(struct sock_addr *)&server, sizeof(server));

if(result < 0){
perror("Packet couldn't sended");
}
printf("Xmas Scan packet sent to %s: %d\n, target, port);

}

struct sockaddr_in from;  // Gelen verinin kaynağını tutacak yapı
socklen_t fromlen = sizeof(from);

char response[BUFFER_SIZE];  // Gelen veriyi tutacak dizi
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT// 1 saniye
	timeout.tv_usec = 0;
	
	int time_exceed_test = setsockopt(r_soket, SOL_SOCKET, SO_RCTIMEO, &timeout, sizeof(timeot));
		if(time_exceed_test < 0){
			perror("Socket option setting failed..\n");
			close(r_socket);
			exit(EXIT_FAILURE);
		}

	


char response[BUFFER_SIZE];
struct sockaddr_in from;
socklen_t fromlen = sizeof(from);

int received_bytes = recvfrom(r_socket, response, sizeof(response), 0, (struct sockaddr *)from,&fromlen);
	if(received_bytes < 0){
		perror("No recving packet loss...\n");
		exit(FAILURE);
	}



iphd *header_of_recv_ip = (iphd *)response;
tcphd *header_of_recv_tcp = (tcphd *)(response + sizeof(iphd));

if(header_of_recv_tcp == 0x04 || header_of_recv_tcp == 0x14){
	//	printf("Port : %d CLOSED  \n", port);
	return 0;
}else {
//	printf("Port: %d Assuming OPEN...\n");
	return 1;
}
close(r_socket);
}


void scan(char *ip){
	for(int i = 1; i < 1024;i++){
		if(tcp_xmas_scan(ip, i)){
			printf(" %d port is open..\n", i);
		}
	
	}


}
