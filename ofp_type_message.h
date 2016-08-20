#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openflow_1_3.h"


void ofph_init(struct ofp_header *h, int type, int len, int xid);
void init_hello(void **buferp) ;
void feat_req(void **buferp);
void feat_reply(void **buferp,int xid,int cn_fd); 
void echo_req(void **buferp);
void barrier_req(void **buferp,int sock);
void get_config_req(void **buferp,int sw_fd);
void echo_reply(void **buferp,int xid); 
void config_set(void **buferp,int sw_fd);
void barrier_reply(void **buferp,int xid,int sock);
void inform_controller_stastu(int virtual_port,int type,int cntl_sockfd);
void of_port_stastu(char* buffer,int buf_len,int cntl_sockfd);////////////////
int proxy_sockfd_covert(int sockfd, int cntl_sockfd,int ofsw_sockfd);
void port_mod(char* buffer, int buf_len,int ofsw_sockfd);
void Flow_removed_handle(char* buffer,int buf_len,int cntl_sockfd);

