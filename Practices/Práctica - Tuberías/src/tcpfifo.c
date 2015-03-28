#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> 	/*Define la familia de direcciones de internet*/
#include <netinet/tcp.h>   	/*Definiciones para encabezados TCP*/
#include <netinet/ip.h>    	/*Definiciones para encabezados IP*/
#include <netinet/if_ether.h>  	/*Mapea una dirección IP multicast a Ethernet multicast*/
#include <net/ethernet.h>  	/*Definiciones para encabezados Ethernet*/
#include <sys/socket.h>		/*Manipulacion de Sockets*/
#include <arpa/inet.h>		/*Definiciones para operaciones de internet*/
#include <sys/ioctl.h>

struct sockaddr_in source,dest;

int main(int argc, char *argv[])
{
	int fd; /* Descriptor del FIFO*/
	int len; /* Bytes escritos en el FIFO*/
	char buf[PIPE_BUF]; /* Se asegurs de escrituras atomicas*/
	time_t tp;

	/************************************************************/
	/*Creacioón de variables que almacenan la direccion de 
 	origen y el tamaño de datos. */
 	int saddr_size , data_size;

 	/*Crea la estructura de socket */
 	struct sockaddr saddr;

 	/*Crea un buffer para almacenar el mensaje 65536 bytes*/
 	unsigned char *buffer = (unsigned char *) malloc(65536);
 	/************************************************************/

 	/*Crea un socket RAW*/
 	int sock_raw = socket( AF_PACKET, /*Corresponde a capa 2*/
				SOCK_RAW, /*Conector puro #3*/
 				htons(ETH_P_ALL));/*Para recibir todos los protocolos*/

 	/*Comprobando la apertura correcta del socket*/
 	if(sock_raw < 0)
    {
        perror("Socket Error");
        return 1;
    }


	/* Nos identificaremos*/
	printf("Starting... I am %d\n", getpid());

	/* Abre el FIFO de sólo escritura*/
	if ((fd = open("fifo1", O_WRONLY)) < 0)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Genera algunos datos para escribir*/
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

        struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));

        if(iph->protocol == 6)
        {
        	unsigned short iphdrlen;
     
		    struct iphdr *iph = (struct iphdr *)( buffer  + sizeof(struct ethhdr) );
		    iphdrlen = iph->ihl*4;
		     
		    struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
		             
		    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
		             
		    memset(&source, 0, sizeof(source));
		    source.sin_addr.s_addr = iph->saddr;
		     
		    memset(&dest, 0, sizeof(dest));
		    dest.sin_addr.s_addr = iph->daddr;

		    //printf("\n\n   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    		//printf("   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));

    		len = sprintf(buf, "tcpfifo %d sends sIP: %s and dIP %s\n", getpid(), 
    																  inet_ntoa(source.sin_addr),
    																  inet_ntoa(dest.sin_addr));
        }
		/* Obtiene la hora actual*/
		//time(&tp);

		/*Crea la cadema a escribir*/
		//len = sprintf(buf, "wrfifo %d sends %s", getpid(), ctime(&tp));

		/*
		* Utiliza (len + 1) por que sprintf no cuenta 
		* el null final
		*/

		if((write(fd, buf, len+1)) < 0)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
		sleep(3);
		exit(EXIT_SUCCESS);
}
