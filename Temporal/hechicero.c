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

#define DHCP_CLIENT_PORT        30000      
#define DHCP_SERVER_PORT        30001

int main() 
{
    int on=1;
    struct sockaddr_in servaddr,cliaddr,rservaddr;
    int sockfd,listenfd,connfd;

/* -------------- Cliente ---------------------- */
    bzero(&cliaddr,sizeof(cliaddr));    
    cliaddr.sin_port = htons(DHCP_CLIENT_PORT);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr =  inet_addr("255.255.255.255");
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) 
        perror("No se puede crear socket Cliente");
/* -------------- Cliente ---------------------- */

    while(1)
    {
    /*------Fill packet---------*/
        //La direccion se debe de cambiar por cada dispositivo
        inet_aton("10.2.18.113", &recvdhcpmsg.yiaddr);
        recvdhcpmsg.op = 1;
        recvdhcpmsg.htype = 1;
        recvdhcpmsg.opt[0] = 53;
        recvdhcpmsg.opt[1]=1;
        recvdhcpmsg.opt[2]=5;
        recvdhcpmsg.opt[3] = 255;
    /*------Fill packet---------*/
    /* ------- Sending ----------------*/
        printf("Enviando datos al Zombie\n");

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