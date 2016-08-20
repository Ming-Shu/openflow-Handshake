#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include "openflow_1_3.h"
 

#define CNTL_IP "140.96.96.80"
#define KEEPALIFETIME 4000
#define SOCKET_PORT 8888
#define P_CNTL_PORT 6633
#define P_OFSW_PORT 6666
#define FILESOCKNAME "/OnuEventSocket"
#define INIT 0
enum connection_type{
	HANDSHARK	=0,
	HANDLE_MESSAGE	=1,
	PROXY_CNTL	=2,
	PROXY_OFSW	=3,
};

enum proxy_module_status{
	INITIAL	=0,
	ACCEPT 	=1,
	CONNECT	=2,
	RECONNECT=3,
	READY	=4,
	ONU_INDICATION=5,
};

enum return_status{
	SUCCESS	=0,
	FAIL 	=-1,
	TIMEOUT	=-2,
	ERROR	=-3,
};

enum flags{
	TRUES	=0,
	FALSES 	=-1,
};
/*
struct connect_info
{
	int sock;
	struct sockaddr_in *addr;
	int addr_len;
};*/

static inline uint64_t htonll(uint64_t n)
{
	return htonl(1)==1 ? n :((uint64_t) htonl(n) << 32 | htonl(n >> 32)); 	
}

static inline uint64_t ntohll(uint64_t n)
{
	return htonl(1)==1 ? n :((uint64_t) ntohl(n) << 32 | ntohl(n >> 32)); 	
}

int proxy_cntl_channel_init(int);
int proxy_ofsw_channel_init();
int connect_to_cntl(int sock);
int connect_to_ofsw(int sock);
void unxi_domain_init();
void onu_indication_alarm();
void onu_indicat_msg();
int select_set_config(int cntl_fd,int ofsw_fd);

void Opfp_channel_connet(int proxy_sock,int proxy_type,int send_sock);
int Handshake_with_switch(int sw_fd,int state);
int Handshake_with_controller(int cntl_fd,int state);
int Communication_message(int sockfd);
void set_nonblocking(int sockid);
void sig_pipe(int singal);
//void onu_indicat_msg();
//void connection_init(int sw_fd);
