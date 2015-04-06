#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>

struct dhcpmessage
{
    uint8_t op;     //Message opcode/type
    uint8_t htype;  //Hardware addr type
    uint8_t hlen;   //Hardware addr length
    uint8_t hops;   //Number of relay agent hops from client
    uint32_t xid;   //Transaction ID
    uint16_t secs;  //Seconds since client started looking
    uint16_t flags; //Flags bits
    //uint32_t ciaddr;//Client IP address (if already in use)
    //uint32_t yiaddr;//Client IP address (if already in use)
    //uint32_t siaddr;//Server IP address (if already in use)
    //uint32_t giaddr;//Client IP address (if already in use)
    struct in_addr ciaddr;//Client IP address (if already in use)
    struct in_addr yiaddr;//Client IP address (if already in use)
    struct in_addr siaddr;//Client IP address (if already in use)
    struct in_addr giaddr;//Client IP address (if already in use)
    unsigned char chaddr[16];//Client hardware address
    char sname[64]; //Server name
    char file[128]; //Boot file name
    char magic[4];  //
    char opt[4];    //Optional parameters (actual length dependent on MTU)
};

#define DHCP_CLIENT_PORT        68      
#define DHCP_SERVER_PORT        67

void filldhcp(struct dhcpmessage *);
void printDhcpMsg(struct dhcpmessage *);


int main() 
{
    int on=1;
    struct sockaddr_in servaddr,cliaddr,rservaddr; //Create struct sockaddr_in

    int sockfd,listenfd,connfd;

/* -------------- Cliente ---------------------- */
    //Fill struct to 0
    bzero(&cliaddr,sizeof(cliaddr));    
    //Configure structs sockaddr_in
    cliaddr.sin_port = htons(DHCP_CLIENT_PORT);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr =  inet_addr("255.255.255.255");
    
    //Create socket
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) 
        perror("Can't create socket");

    //if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
      //  printf("setsockopt"); 
    /*if(setsockopt(  sockfd,
                    SOL_SOCKET,
                    SO_BROADCAST,
                    &on,
                    sizeof(on)
                 ) < 0)
        printf("setsockopt");
/* -------------- Cliente ---------------------- */

/* -------------- Servidor ---------------------- */
    //Fill struct to 0
    bzero(&servaddr,sizeof(servaddr));  
    //Configure structs sockaddr_in
    servaddr.sin_port = htons(DHCP_SERVER_PORT);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //Create socket
    if((listenfd=socket(AF_INET,SOCK_DGRAM,0)) < 0) 
        perror("Can't create socket");

    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        perror("bind");    

/* -------------- Servidor ---------------------- */

/* -------Create message to send----------------*/
    struct dhcpmessage dhcpmsg;
    bzero(&dhcpmsg,sizeof(dhcpmsg));
    filldhcp(&dhcpmsg);
/* -------Create message to send----------------*/
    while(1)
    {
    /* ------- Receiving ----------------*/
        printf("Recibiendo datos del servidor\n");
        struct dhcpmessage recvdhcpmsg;
        bzero(&recvdhcpmsg,sizeof(recvdhcpmsg));
        int serv_len = sizeof(servaddr);
        if(recvfrom(listenfd,
                    &recvdhcpmsg,
                    sizeof(recvdhcpmsg),
                    0,
                    (struct sockaddr*)&servaddr,
                    &serv_len) < 0)
            perror("Can't receive");

        printDhcpMsg(&recvdhcpmsg);
    /* ------- Receiving ----------------*/
        /*------Fill packet---------*/
        //La direccion se debe de cambiar por cada dispositivo
        inet_aton("192.168.1.102", &recvdhcpmsg.yiaddr);
        recvdhcpmsg.op = 1;
        recvdhcpmsg.htype = 1;
        recvdhcpmsg.opt[0] = 53;
        recvdhcpmsg.opt[1]=1;
        recvdhcpmsg.opt[2]=5;
        recvdhcpmsg.opt[3] = 255;
        /*------Fill packet---------*/
    /* ------- Sending ----------------*/
        printf("Enviando datos al Servidor\n");

        if(setsockopt(  sockfd,
                    SOL_SOCKET,
                    SO_BROADCAST,
                    &on,
                    sizeof(on)
                 ) < 0)
            printf("setsockopt");
        
        if(sendto(  sockfd,
                    &recvdhcpmsg,
                    sizeof(recvdhcpmsg),
                    0,
                    (struct sockaddr*)&cliaddr, 
                    sizeof(cliaddr)
                ) < 0)
            printf("sendto");
        printDhcpMsg(&recvdhcpmsg);
    /* ------- Sending ----------------*/
    }
    close(sockfd);
    return 0;
}


void filldhcp(struct dhcpmessage * dhcpmsg)
{
    dhcpmsg->op = 1;
    dhcpmsg->htype = 2;
    dhcpmsg->hlen = 6;
    dhcpmsg->hops = 0;
    dhcpmsg->xid = htonl(1000);
    dhcpmsg->secs = htons(0);
    dhcpmsg->flags = htons(0x8000);
    //dhcpmsg->ciaddr = 0;
    //dhcpmsg->yiaddr = 0;
    //dhcpmsg->siaddr = 0;
    //dhcpmsg->giaddr = 0;
    inet_aton("0.0.0.0", &dhcpmsg->ciaddr);
    inet_aton("0.0.0.0", &dhcpmsg->yiaddr);
    inet_aton("0.0.0.0", &dhcpmsg->siaddr);
    inet_aton("0.0.0.0", &dhcpmsg->giaddr);
    dhcpmsg->chaddr[0] = 0x00;
    dhcpmsg->chaddr[1] = 0x1A;
    dhcpmsg->chaddr[2] = 0x80;
    dhcpmsg->chaddr[3] = 0x80;
    dhcpmsg->chaddr[4] = 0x2C;
    dhcpmsg->chaddr[5] = 0xC3;
    dhcpmsg->magic[0]=99;
    dhcpmsg->magic[1]=130;
    dhcpmsg->magic[2]=83;
    dhcpmsg->magic[3]=99;
    dhcpmsg->opt[0]=53;
    dhcpmsg->opt[1]=1;
    dhcpmsg->opt[2]=1;
    dhcpmsg->opt[3]=255;
}

void printDhcpMsg(struct dhcpmessage * message)
{
    printf("=========================\n");
    printf("OP:     \t%i\n", message->op);
    printf("HTYPE:  \t%i\n", message->htype);
    printf("HLEN:   \t%i\n", message->hlen);
    printf("HOPS:   \t%i\n", message->hops);
    printf("XID:    \t%i\n", message->xid);
    printf("SECS:   \t%i\n", message->secs);
    printf("FLAGS:  \t%i\n", message->flags);
    //printf("CIADDR: \t%i\n", message->ciaddr);
    //printf("YIADDR: %i\t\n", message->yiaddr);
    //printf("SIADDR: %i\t\n", message->siaddr);
    //printf("GIADDR: %i\t\n", message->giaddr);
    printf("CIADDR: \t%s\n", inet_ntoa(message->ciaddr));
    printf("YIADDR: \t%s\n", inet_ntoa(message->yiaddr));
    printf("SIADDR: \t%s\n", inet_ntoa(message->siaddr));
    printf("GIADDR: \t%s\n", inet_ntoa(message->giaddr));
    printf("MACADD: \t%02x:%02x:%02x:%02x:%02x:%02x\n", message->chaddr[0], 
                                                        message->chaddr[1], 
                                                        message->chaddr[2], 
                                                        message->chaddr[3], 
                                                        message->chaddr[4], 
                                                        message->chaddr[5]);
    printf("=========================\n");
}