#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

using namespace std;

/*Compilar con la siguiente directiva:
    g++ -std=c++11 mapport.cxx mapport*/

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Falta la IP");
        return 0;
    }
    else
    {
        ifstream ifile ("protlist.csv");
        string sSokect, sAux;

        if(ifile.is_open())
        {
            char ch[20];
            while(!ifile.eof())
            {
                ifile >> sAux;
                size_t stToken = sAux.find(",");
                sSokect += sAux.substr(0, stToken);
                sSokect += sAux.substr(stToken);

            /*Inicio_estado del puerto*/
                int portno; //= "3306";
                string::size_type sz;
                stToken++;
                portno = std::stoi (sAux.substr(stToken),&sz);
                //char hostnameb[16] = "192.168.1.150";
                char *hostname = argv[1];
                /*char *hostname = hostnameb;*/

                int sockfd;
                struct sockaddr_in serv_addr;
                struct hostent *server;

                /*Crea un socket no ligado a un dominio.Devuelve un descriptor.
                Parametros: domain (Especifica e dominio de comunicaciones en el cual el 
                soket sera creado), type (Especifica el tipo de socket a crear), 
                protocol (Especifica el protocolo a ser usado, en 0 el socket usa el 
                protocolo adecuado para la situación). Usar SOCK_STREAM para TCP o 
                SOCK_DGRAM para UDP.*/
                string sTipoSokect = argv[1];
                if(sTipoSokect.compare("TCP"))
                {
                    sockfd = socket(AF_INET, SOCK_STREAM, 0);
                }
                else
                {
                    if(sTipoSokect.compare("UDP"))
                    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
                    else
                    {
                        printf("%c\n", "Error en el paramtro del protocolo");   
                        return 0;
                    }
                }

                /*Devuelve un entero no negativo en caso de que la operacion de creacióm
                del socket se realice correctamente, de otra manera devuelve -1*/
                if (sockfd < 0) 
                {
                    perror("ERROR opening socket");
                }

                server = gethostbyname(hostname);

                if (server == NULL) 
                {
                    fprintf(stderr,"ERROR, no such host\n");
                    exit(0);
                }

                bzero((char *) &serv_addr, sizeof(serv_addr));
                serv_addr.sin_family = AF_INET;
                bcopy((char *)server->h_addr, 
                     (char *)&serv_addr.sin_addr.s_addr,
                     server->h_length);

                serv_addr.sin_port = htons(portno);
                if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
                {
                    //printf("Port is closed");
                    sSokect += "     ->    closed\n";
                } else 
                {
                    //printf("Port is active");
                    sSokect += "     ->    active\n";
                }
                close(sockfd);
            /*Fin_estado del puerto*/
            }
        }
        else
        {
            printf("Error abriendo..\n");
        }
        ifile.close();
        cout << sSokect << endl;
        return 0;
    }
}