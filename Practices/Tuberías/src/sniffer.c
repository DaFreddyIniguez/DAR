/*#include <netinet> Estructuras y funciones de direcciones de dominio de internet*/

#include <errno.h> /*Define macros que reciben y reportan errores*/ 
#include <netdb.h> /*Definiciones para operaciones de base de datos de red*/
#include <stdio.h> /*Define 3 tipos de variables, varias macros y varias funciones para 
						desempeñar la entrada y salida.*/
#include <stdlib.h> /*Define 4 tipos de variables, varias macros y funciones para dsempeñar
					funciones generales. Por ejemplo: malloc: "Permite solicitar memoria al 
					sistema devolviendo un apuntador a esta."*/
#include <string.h>    /*Define un tipo de variable, una macro y varias funciones para
						 manipular arreglos de caracteres. Por ejemplo: strlen "Obtine
						 la longitud de un arreglo"*/
#include <netinet/in.h> /*Define la familia de direcciones de internet*/
#include <netinet/ip_icmp.h>  /*Definiciones del protocolo ICMP.*/
#include <netinet/udp.h>   /*Definiciones para encabezados UDP*/
#include <netinet/tcp.h>   /*Definiciones para encabezados TCP*/
#include <netinet/ip.h>    /*Definiciones para encabezados IP*/
#include <netinet/if_ether.h>  /*Mapea una dirección IP multicast a Ethernet multicast*/
#include <net/ethernet.h>  /*Definiciones para encabezados Ethernet*/
#include <sys/socket.h>		/*Manipulacion de Sockets*/
#include <arpa/inet.h>	/*Definiciones para operaciones de internet*/
#include <sys/ioctl.h>	/*Manipula los parámetros subyacentes de archivos especiales*/
#include <sys/time.h>	/*Tipos de tiempo*/
#include <sys/types.h>	/*Tipos de datos. Por ejemplo los usados en sys/socket.h y
						 netinet/in.h */
#include <unistd.h> /*Cabecera define constantes y tipos simbólicos diversos, y 
						declara funciones auxiliares*/
//////////////////////////////////////
/*Funciones de utilidad*/
void ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void PrintData (unsigned char* , int);

/////////////////////////////////////
//FILE *logfile;

 /*Define estructuras de sockets origen y destino*/
 struct sockaddr_in source,dest;

/*Varibles contadores de paquetes cachados*/
 int tcp = 0, udp = 0, icmp = 0, others = 0, igmp = 0, total = 0, i, j;


 int main(int argc, char *argv[])
 {
 	/*Creacioón de variables que almacenan la direccion de 
 	origen y el tamaño de datos. */
 	int saddr_size , data_size;

 	/*Crea la estructura de socket */
 	struct sockaddr saddr;

 	/*Crea un buffer para almacenar el mensaje 65536 bytes*/
 	unsigned char *buffer = (unsigned char *) malloc(65536);

 	/*logfile=fopen("log.txt","w");
    if(logfile==NULL) 
    {
        printf("Unable to create log.txt file.");
    }*/


 /*Inicia captura de paquetes*/
 	printf("Starting...\n");

 	/*Crea un socket RAW*/
 	int sock_raw = socket( AF_PACKET, /*Corresponde a capa 2*/
 							SOCK_RAW, /*Conector puro #3*/
 							htons(ETH_P_ALL));/*Para recibir todos 
 												los protocolos*/

 	/*Comprobando la apertura correcta del socket*/
 	if(sock_raw < 0)
    {
        perror("Socket Error");
        return 1;
    }
    /*Ciclo que recibe datos del socket abierto*/
    while(1)
    {
    	/*Obtiene el tamaño de momoria de la estructura */
        saddr_size = sizeof saddr;
        /*Recibe paquetes:  recvfrom(int sockfd, 
                                     void *buf, 
                                     size_t len, 
                                     int flags,
                 					 struct sockaddr *src_addr, 
                 					 socklen_t *addrlen);*/
        data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
        /*Comprobar que se reciben datos*/
        if(data_size <0 )
        {
            printf("Recvfrom error , failed to get packets\n");
            return 1;
        }
        /*Se procesa el paquete enviando buffer y el tamaño*/
        ProcessPacket(buffer , data_size);
    }
    /*Cerrando socket*/
 	close(sock_raw);
    printf("Finished");
    return 0;
 }


void ProcessPacket(unsigned char* buffer, int size)
{
    /*Obtiene la parte del encabezado de IP de este paquete, excluyendo el 
    encabezado de Ethernet*/
/*////////////////////////////////////
 __u8    tos;
 __u16   tot_len;
 __u16   id;
 __u16   frag_off;
 __u8    ttl;
 __u8    protocol;
 __u16   check;
 __u32   saddr;
 __u32   daddr;
*///////////////////////////////////////
    struct iphdr *iph = (struct iphdr*)(buffer + 
    						sizeof(struct ethhdr));
    /*Aumenta el numero de paquetes procesados*/
    ++total;

    /*Revisa el protocolo, aumentando su contador segun sea el tipo de protocolo, y
    se imprimen los contadores*/
    /*Apunta a la estructura en protocolo*/
    switch (iph->protocol)
    {
        case 1:  //ICMP Protocol
            ++icmp;
            print_icmp_packet( buffer , size);
            break;
         
        case 2:  //IGMP Protocol
            ++igmp;
            break;
         
        case 6:  //TCP Protocol
            ++tcp;
            print_tcp_packet(buffer , size);
            break;
         
        case 17: //UDP Protocol
            ++udp;
            print_udp_packet(buffer , size);
            break;
         
        default: //Some Other Protocol like ARP etc.
            ++others;
            break;
    }
    printf("\nTCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r",
                   tcp ,       udp ,         icmp ,         igmp ,        others ,      total);
}

void print_ethernet_header(unsigned char* Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
    /* 
    fprintf(logfile , "\n");
    fprintf(logfile , "Ethernet Header\n");
    fprintf(logfile , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    fprintf(logfile , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    fprintf(logfile , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
	*/
    printf("\n\n\n");
    printf("Ethernet Header\n");
    printf("MAC: *******************\n   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    printf("********************++++\n   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}
 
void print_ip_header(unsigned char* Buffer, int Size)
{
    print_ethernet_header(Buffer , Size);
   
    unsigned short iphdrlen;
         
    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
    iphdrlen =iph->ihl*4;
     
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
     /*
    fprintf(logfile , "\n");
    fprintf(logfile , "IP Header\n");
    fprintf(logfile , "   |-IP Version        : %d\n",(unsigned int)iph->version);
    fprintf(logfile , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    fprintf(logfile , "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    fprintf(logfile , "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    fprintf(logfile , "   |-Identification    : %d\n",ntohs(iph->id));
    //fprintf(logfile , "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //fprintf(logfile , "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //fprintf(logfile , "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    fprintf(logfile , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
    fprintf(logfile , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
    fprintf(logfile , "   |-Checksum : %d\n",ntohs(iph->check));
    fprintf(logfile , "   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    fprintf(logfile , "   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
    */
    printf("\n\n   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    printf("   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}
 
void print_tcp_packet(unsigned char* Buffer, int Size)
{
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
    iphdrlen = iph->ihl*4;
     
    struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));
             
    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
     
    //fprintf(logfile , "\n\n***********************TCP Packet*************************\n");  
         
    print_ip_header(Buffer,Size);
    /*     
    fprintf(logfile , "\n");
    fprintf(logfile , "TCP Header\n");
    fprintf(logfile , "   |-Source Port      : %u\n",ntohs(tcph->source));
    fprintf(logfile , "   |-Destination Port : %u\n",ntohs(tcph->dest));
    fprintf(logfile , "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    fprintf(logfile , "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    fprintf(logfile , "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    //fprintf(logfile , "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
    //fprintf(logfile , "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
    fprintf(logfile , "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    fprintf(logfile , "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    fprintf(logfile , "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    fprintf(logfile , "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    fprintf(logfile , "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    fprintf(logfile , "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    fprintf(logfile , "   |-Window         : %d\n",ntohs(tcph->window));
    fprintf(logfile , "   |-Checksum       : %d\n",ntohs(tcph->check));
    fprintf(logfile , "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    fprintf(logfile , "\n");
    fprintf(logfile , "                        DATA Dump                         ");
    fprintf(logfile , "\n");
         
    fprintf(logfile , "IP Header\n");
    PrintData(Buffer,iphdrlen);
         
    fprintf(logfile , "TCP Header\n");
    PrintData(Buffer+iphdrlen,tcph->doff*4);
         
    fprintf(logfile , "Data Payload\n");    
    PrintData(Buffer + header_size , Size - header_size );
                         
    fprintf(logfile , "\n###########################################################");
    */
}
 
void print_udp_packet(unsigned char *Buffer , int Size)
{
     
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;
     
    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));
     
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
     
    //fprintf(logfile , "\n\n***********************UDP Packet*************************\n");
     
    print_ip_header(Buffer,Size);           
     /*
    fprintf(logfile , "\nUDP Header\n");
    fprintf(logfile , "   |-Source Port      : %d\n" , ntohs(udph->source));
    fprintf(logfile , "   |-Destination Port : %d\n" , ntohs(udph->dest));
    fprintf(logfile , "   |-UDP Length       : %d\n" , ntohs(udph->len));
    fprintf(logfile , "   |-UDP Checksum     : %d\n" , ntohs(udph->check));
     
    fprintf(logfile , "\n");
    fprintf(logfile , "IP Header\n");
    PrintData(Buffer , iphdrlen);
         
    fprintf(logfile , "UDP Header\n");
    PrintData(Buffer+iphdrlen , sizeof udph);
         
    fprintf(logfile , "Data Payload\n");    
     
    //Move the pointer ahead and reduce the size of string
    PrintData(Buffer + header_size , Size - header_size);
     
    fprintf(logfile , "\n###########################################################");
    */
}
 
void print_icmp_packet(unsigned char* Buffer , int Size)
{
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;
     
    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));
     
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;
     
    //fprintf(logfile , "\n\n***********************ICMP Packet*************************\n"); 
     
    print_ip_header(Buffer , Size);
    /*
    fprintf(logfile , "\n");
         
    fprintf(logfile , "ICMP Header\n");
    fprintf(logfile , "   |-Type : %d",(unsigned int)(icmph->type));
             
    if((unsigned int)(icmph->type) == 11)
    {
        fprintf(logfile , "  (TTL Expired)\n");
    }
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    {
        fprintf(logfile , "  (ICMP Echo Reply)\n");
    }
     
    fprintf(logfile , "   |-Code : %d\n",(unsigned int)(icmph->code));
    fprintf(logfile , "   |-Checksum : %d\n",ntohs(icmph->checksum));
    //fprintf(logfile , "   |-ID       : %d\n",ntohs(icmph->id));
    //fprintf(logfile , "   |-Sequence : %d\n",ntohs(icmph->sequence));
    fprintf(logfile , "\n");
 
    fprintf(logfile , "IP Header\n");
    PrintData(Buffer,iphdrlen);
         
    fprintf(logfile , "UDP Header\n");
    PrintData(Buffer + iphdrlen , sizeof icmph);
         
    fprintf(logfile , "Data Payload\n");    
     
    //Move the pointer ahead and reduce the size of string
    PrintData(Buffer + header_size , (Size - header_size) );
     
    fprintf(logfile , "\n###########################################################");
    */
}
 
void PrintData (unsigned char* data , int Size)
{/*
    int i , j;
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            fprintf(logfile , "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    fprintf(logfile , "%c",(unsigned char)data[j]); //if its a number or alphabet
                 
                else fprintf(logfile , "."); //otherwise print a dot
            }
            fprintf(logfile , "\n");
        } 
         
        if(i%16==0) fprintf(logfile , "   ");
            fprintf(logfile , " %02X",(unsigned int)data[i]);
                 
        if( i==Size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++) 
            {
              fprintf(logfile , "   "); //extra spaces
            }
             
            fprintf(logfile , "         ");
             
            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) 
                {
                  fprintf(logfile , "%c",(unsigned char)data[j]);
                }
                else
                {
                  fprintf(logfile , ".");
                }
            }
             
            fprintf(logfile ,  "\n" );
        }
    }
    */
}