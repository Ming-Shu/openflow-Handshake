#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include "openflow_1.3.3.h"

#include"channel_communication.h"
#include "ofp_type_message.h"
#include "oxm_match.h"
#include "flow.h"

void ofph_init(struct ofp_header *h, int type, int len, int xid) 
{
  h->version = OFP_VERSION;
  h->type = type;
  h->length = htons(len);
  h->xid = xid;
}

void init_hello(void **buferp) 
{
  printf("Sending a 'OFPT_HELLO' message ....\n");  
  struct ofp_hello *p;
  *buferp = malloc(sizeof(struct ofp_hello));
  p = *(struct ofp_hello **)buferp;
  ofph_init(&p->header,OFPT_HELLO, sizeof(struct ofp_hello),0);
}

void feat_req(void **buferp) 
{
  printf("Sending a 'OFPT_FEATURES_REQUEST' message to switch....\n");
  struct ofp_hello *p;
  *buferp = malloc(sizeof(struct ofp_hello));
  p = *(struct ofp_hello **)buferp;
  ofph_init(&p->header, OFPT_FEATURES_REQUEST, sizeof(struct ofp_hello),1);
}

void feat_reply(void **buferp,int xid,int cn_fd) 
{
  printf("Sending a 'OFPT_FEATURES_REPLY' message to controller...\n");
  struct ofp_switch_features *p;
  *buferp = malloc(sizeof(struct ofp_switch_features));
  p = *(struct ofp_switch_features **)buferp;
  ofph_init(&p->header, OFPT_FEATURES_REPLY, sizeof(struct ofp_switch_features),xid);
  p->datapath_id  = 	htonll( 123546789 );
  p->n_buffers 	  =	htonl(256);
  p->auxiliary_id = 1;	
  p->capabilities = htonl(OFPC_FLOW_STATS|OFPC_TABLE_STATS|OFPC_PORT_STATS|OFPC_IP_REASM );
  send(cn_fd,p,sizeof(struct ofp_switch_features),0); 	
}

void echo_req(void **buferp) 
{
  printf("Sending a 'OFPT_ECHO_REQUEST' message ....\n");
  static int xid=10;		
  struct ofp_hello *p;
  *buferp = malloc(sizeof(struct ofp_hello));
  p = *(struct ofp_hello **)buferp;
  ofph_init(&p->header,OFPT_ECHO_REQUEST, sizeof(struct ofp_hello),xid);
  xid++;	
}

void echo_reply(void **buferp,int xid) 
{
  printf("Sending a 'OFPT_ECHO_REPLY' message ....\n");
  struct ofp_hello *p;	
  *buferp = malloc(sizeof(struct ofp_hello));
  p = *(struct ofp_hello **)buferp;
  ofph_init(&p->header,OFPT_ECHO_REPLY, sizeof(struct ofp_hello),xid);
}

void barrier_req(void **buferp,int sock) 
{
  printf("Sending a 'OFPT_BARRIER_REQUEST' message ....\n");
  struct ofp_hello *p;
  *buferp = malloc(sizeof(struct ofp_hello));
  p = *(struct ofp_hello **)buferp;
  ofph_init(&p->header, OFPT_BARRIER_REQUEST, sizeof(struct ofp_hello),3);
  send(sock,p,sizeof(struct ofp_hello),0);	
}

void barrier_reply(void **buferp,int xid,int sock) 
{
  struct ofp_hello *p;
  *buferp = malloc(sizeof(struct ofp_hello));
  p = *(struct ofp_hello **)buferp;
  ofph_init(&p->header, OFPT_BARRIER_REPLY, sizeof(struct ofp_hello),xid);
  send(sock,p,sizeof(struct ofp_hello),0);	
}

void get_config_req(void **buferp,int sw_fd) 
{
  printf("Sending a 'get_config_req' message to switch....\n");
  struct ofp_switch_config *p;
  *buferp = malloc(sizeof(struct ofp_switch_config));
  p = *(struct ofp_switch_config **)buferp;
  ofph_init(&p->header, OFPT_GET_CONFIG_REQUEST, sizeof(struct ofp_switch_config),4);
  p->flags = OFPC_FRAG_NORMAL;
  p->miss_send_len =htons(OFP_DEFAULT_MISS_SEND_LEN);
  send(sw_fd,p,sizeof(struct ofp_switch_config),0);	
}

void config_set(void **buferp,int sw_fd) 
{
  printf("Sending a 'config_set' message to switch....\n");
  struct ofp_switch_config *p;
  	
  *buferp = malloc(sizeof(struct ofp_switch_config));
  p = *(struct ofp_switch_config **)buferp;
  ofph_init(&p->header,OFPT_SET_CONFIG, sizeof(struct ofp_switch_config),5);
  p->flags = OFPC_FRAG_NORMAL;
  p->miss_send_len =htons(OFP_DEFAULT_MISS_SEND_LEN);
  send(sw_fd,p,sizeof(struct ofp_switch_config),0);
}

void inform_controller_stastu(int virtual_port,int type,int cntl_sockfd)
{
  printf("-----------------------Sending a 'port change info' message to controller-----------------\n\n");
  //printf("virtual_port:%d,type:%d\n\n",virtual_port,type);	
  struct ofp_port_status *p;		
  struct ofp_port_status pStastu;
  p=&pStastu;	
  ofph_init(&p->header,OFPT_PORT_STATUS, sizeof(struct ofp_port_status),6);	
  p->reason = (type==INDCT_ADD)?OFPPR_ADD:OFPPR_DELETE;
  p->desc.port_no =  htonl(virtual_port);
  strcpy(p->desc.hw_addr,"41:98:91:35:75:21");
  strcpy(p->desc.name,"ONU_CH");
  p->desc.config = (type==INDCT_ADD)?0:htonl(OFPPC_PORT_DOWN);                   
  p->desc.state = htonl(OFPPS_LINK_DOWN);                    
  p->desc.curr = htonl(OFPPF_100MB_FD | OFPPF_FIBER | OFPPF_AUTONEG);                   
  p->desc.advertised = htonl(OFPPF_10MB_HD | OFPPF_10MB_FD | OFPPF_100MB_HD | OFPPF_100MB_FD | OFPPF_FIBER | OFPPF_AUTONEG);               
  p->desc.supported = htonl(OFPPF_10MB_HD | OFPPF_10MB_FD | OFPPF_100MB_HD | OFPPF_100MB_FD | OFPPF_1GB_FD | OFPPF_FIBER | OFPPF_AUTONEG);                
  p->desc.peer =0;                     
  p->desc.curr_speed = htonl(OFPPF_100MB_FD);             
  p->desc.max_speed  = htonl(OFPPF_100MB_FD);               
  send(cntl_sockfd,p,sizeof(struct ofp_port_status),0);		
}

void of_port_stastu(char* buffer,int buf_len,int cntl_sockfd)
{
  printf("------------------Staring handle 'Port Stastu' message from switch-------------\n\n");
  int port,virtual_port;
  struct ofp_port_status *p;
  p = (struct ofp_port_status*)buffer;	
  printf("The Port=%d Change\n",htonl(p->desc.port_no));
  port = htonl(p->desc.port_no);
  if(port>OF_OLT_CONNT_NUM )
     {
	  printf("\n The port>OF_OLT_CONNT_NUM .....\n");
   	  virtual_port = of_virtual_port(port);
	  printf("The New Port=%d \n",virtual_port);
	   p->desc.port_no =  htonl(virtual_port);
	  send(cntl_sockfd,p,sizeof(struct ofp_port_status),0);
     }else{ 
           printf("\n The port<= OF_OLT_CONNT_NUM.....\n"); 
  	   printf("\n The Machine link error!\n\n"); 
     }//else
  
}//of_port_stastu

int proxy_sockfd_covert(int sockfd, int cntl_sockfd,int ofsw_sockfd)
{
    int send_sock;
    printf("--------proxy_sockfd_covert-------\n\n");
	if(sockfd ==cntl_sockfd)
	{
	printf("sockfd == controller_sockfd=%d\n",sockfd);
    send_sock = ofsw_sockfd;
	}
	else
	{
	 printf("sockfd == controller_sockfd=%d\n",sockfd);
	 send_sock = cntl_sockfd;
	}
	printf("return send_sock=%d\n\n",send_sock);
	return send_sock ;
}//proxy_sockfd_covert

void port_mod(char* buffer, int buf_len,int ofsw_sockfd)
{
  int vir_port,init_port;   
  struct ofp_port_mod *port_mod;
  port_mod = (struct ofp_port_mod *)buffer;
  vir_port = htonl(port_mod->port_no);
  if(vir_port>ONU_PORT_NUM)
        init_port = of_init_port(vir_port); 
  else
        init_port = olt_of_port_convert(vir_port); 
        
  port_mod->port_no = htonl(init_port);                                            
  send(ofsw_sockfd,port_mod,buf_len,0);   
}//port_mod

void Flow_removed_handle(char* buffer,int buf_len,int cntl_sockfd)
{
  printf("------------------Staring handle 'Flow_removed' message from switch-------------\n\n");
  int in_port,vlan;
  struct ofp_flow_removed *p,*c;
  struct ofp_match * matchP;
  p = (struct ofp_flow_removed*)buffer;	
  matchP = &p->match;
  in_port = read_match(matchP,OFPXMT_OFB_IN_PORT);	
  printf("in_port :%d\n",in_port);
  printf("table_id :%d\n",p->table_id);
  if(p->table_id==0)
    {   
     if(in_port>OF_OLT_CONNT_NUM )
     {
	  printf("\n in_port>OF_OLT_CONNT_NUM .....\n\n");
   	  in_port = of_virtual_port(in_port);
	  modify_match(matchP,OFPXMT_OFB_IN_PORT,in_port);
	  send(cntl_sockfd,p,buf_len,0);
     }else{ 
           printf("\n table_id==0,in_port <= OF_OLT_CONNT_NUM .....\n\n"); 
           vlan = read_match(matchP,OFPXMT_OFB_VLAN_VID);
           printf("vlan :%d\n",vlan);
     }//else
    }else{
          printf("\n The table_id != 0,DownStrem or ONU Loop entry.....\n\n");
          vlan = read_match(matchP,OFPXMT_OFB_VLAN_VID);
          printf("vlan :%d\n",vlan);
          in_port = p->table_id-1;
          modify_match(matchP,OFPXMT_OFB_IN_PORT,in_port);
       	  send(cntl_sockfd,p,buf_len,0);
    }//else               
}//Flow_removed_handle 
/*
void create_flow_entry(char* buffer,int buf_len)
{

 printf("----------------------Create a flow_entry in table-----------------------\n\n");
 char *flow_entry_add;
 //char *test = " Hello!!! this is test\n";
 struct ofp_flow_mod *flow_mod;
 struct list *flow_entry;
 struct ofp_match* pMatch;
 struct ofp_instruction_actions *inst_actions;
 void*ptr;
 
 flow_entry_add = (char*)malloc(buf_len);
 if(flow_entry_add ==NULL)
	exit(EXIT_FAILURE);

 memcpy(flow_entry_add,buffer,buf_len);
 count++;

 flow_entry = insert_node(flow_entry_add,count);
 //printf("flow_entry->num : %d\n",flow_entry->num);
//print of flow entry info.
 flow_mod =  (struct ofp_flow_mod*)flow_entry->data;
 pMatch = &flow_mod->match;
 flow_mod_offset(&ptr,&pMatch->oxm_fields,htons(pMatch->length));
 inst_actions =(struct ofp_instruction_actions*)ptr;	
 printf("Flow entry num:%d\n\n",flow_entry->num);

 printf("flow entry out_port:%08x\n\n",flow_mod->out_port);
 flow_mod->out_port=2;//test modify out_port 
 printf("Modify flow entry out_port:%08x\n\n",flow_mod->out_port);

 printf("version=%04x, type=%d, length=%d, xid=%d\n\n", flow_mod->header.version, flow_mod->header.type, htons(flow_mod->header.length), htonl(flow_mod->header.xid));

 printf("Flow Mod:cookie=%d,cookie_mask=%d,table_id=%d command=%d\n",htonll(flow_mod->cookie),htonll(flow_mod->cookie_mask),flow_mod->table_id, flow_mod->command);
 printf("idle_timeout=%d,hard_timeout=%d,priority=%d,buffer_id=%d,out_port:%d,,out_group=%d,flags=%d\n",htons(flow_mod->idle_timeout),htons(flow_mod->hard_timeout),htons(flow_mod->priority),htonl(flow_mod->buffer_id),flow_mod->out_port,htonl(flow_mod->out_group),htons(flow_mod->flags));
	
 printf("Flow Match:  type=%d,length=%d\n", htons(pMatch->type), htons(pMatch->length));
 printf("OXM TLV header:CLASS=%d,FIELD=%d,TYPE=%d,HASMASK=%d,LENGTH=%d\n\n", OXM_CLASS(*pMatch->oxm_fields), OXM_FIELD(*(*pMatch).oxm_fields), OXM_TYPE(*(pMatch->oxm_fields)), OXM_HASMASK(*(pMatch->oxm_fields)), OXM_LENGTH(*(pMatch->oxm_fields)));
printf("Flow instruction_actions: type=%d,len=%d,action_type=%d,action_len=%d\n\n",htons(inst_actions->type),htons(inst_actions->len),htons(inst_actions->actions->type),htons(inst_actions->actions->len));
 	 
}//create_flow_entry
*/


/*int flow_mod_handle(char* buffer, int buf_len,int cntl_sockfd,int ofsw_sockfd)
{
 printf("------------------Staring handle 'flow_mod' message from controller-------------\n\n");	
 struct ofp_flow_mod *f;
 struct ofp_match* pMatch;
 struct ofp_instruction_actions *ofp_inst_act;
 void*ptr;		
 f = (struct ofp_flow_mod*)buffer; 	
 pMatch = &f->match;
 flow_mod_offset(&ptr,&pMatch->oxm_fields,htons(pMatch->length));
 ofp_inst_act =(struct ofp_instruction_actions*)ptr;
 /*printf("---------------------test-------------------------------------\n");
  printf("sizeof(struct ofp_flow_mod):%d\n,sizeof(struct ofp_instruction_actions):%d\n\n",sizeof(struct ofp_flow_mod),sizeof(struct ofp_instruction_actions));

 printf("&f->header=%d,&f->match=%d,&pMatch->oxm_fields=%d,ofp_inst_act=%d\n\n",&f->header,&f->match,&pMatch->oxm_fields,ofp_inst_act);
 printf("---------------------test-------------------------------------\n");*/
 /*printf("version=%04x, type=%d, length=%d, xid=%d\n\n", f->header.version, f->header.type, htons(f->header.length), htonl(f->header.xid));
 printf("Flow Mod:cookie=%d,cookie_mask=%d,table_id=%d command=%d,idle_timeout=%d,hard_timeout=%d,priority=%d,buffer_id=%d,out_port=%08x,out_group=%08x,flags=%04x\n\n",htonll(f->cookie),htonll(f->cookie_mask),f->table_id, f->command,htons(f->idle_timeout),htons(f->hard_timeout),htons(f->priority),htonl(f->buffer_id),htonl(f->out_port),htonl(f->out_group),htons(f->flags));	
 printf("Flow Match:  type=%d,length=%d\n", htons(pMatch->type), htons(pMatch->length));
 printf("OXM TLV header:CLASS=%d,FIELD=%d,TYPE=%d,HASMASK=%d,LENGTH=%d\n\n", OXM_CLASS(*pMatch->oxm_fields), OXM_FIELD(*(*pMatch).oxm_fields), OXM_TYPE(*(pMatch->oxm_fields)), OXM_HASMASK(*(pMatch->oxm_fields)), OXM_LENGTH(*(pMatch->oxm_fields)));
printf("Flow instruction_actions: type=%d,len=%d,action_type=%d,action_len=%d\n\n",htons(ofp_inst_act->type),htons(ofp_inst_act->len),htons(ofp_inst_act->actions->type),htons(ofp_inst_act->actions->len));
printf("----------------------Ending handle'flow_mod' message -----------------------\n\n");
printf("Sending a 'ofp_flow_mod' message to switch....\n\n");

send(ofsw_sockfd,buffer,buf_len,0);
 return 0;		
}*/
