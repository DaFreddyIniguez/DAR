#include <stdio.h>
#include <security/pam_appl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


struct pam_response *reply;

int null_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {

        *resp = reply;
        return PAM_SUCCESS;

}

static struct pam_conv conv = { null_conv, NULL };

int main(int argc, char *argv[]) {

        int retval;
        char *user, *pass;  

            struct termios term, term_orig;
            tcgetattr(STDIN_FILENO, &term);
            term_orig = term;
            term.c_lflag &= ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &term);
         
            char buf[1000];
            printf("Usuario ");
            scanf("%s", buf);
            printf(" %s: ", buf);
            user = strdup(buf);
            printf(" Contra:");
            scanf("%s", buf);
            //printf("Read: %s\n", buf);
            pass = strdup(buf);
         
         
            /* Remember to set back, or your commands won't echo! */
            tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
        

        return authenticate("system-auth", user, pass);

}   

int valida(char *s1, char *s2)
{
  int i;
  for (i = 0; s1[i] == s2[i]; i++)
    if (s1[i] == '\0')
      return 0;
  return s1[i] - s2[i];
}

int authenticate(char *service, char *user, char *pass) {
        //
        int fd; /* Descriptor del FIFO*/
        int len; /* Bytes leidos del FIFO*/
        char buf[PIPE_BUF];
        mode_t mode = 0666;
        char arreglo[8]={'0','9','5','1','1','6','6','5'};
        int i=0;
        int leno=8;
        int minimo=0;
        //
        pam_handle_t *pamh = NULL;
        int retval = pam_start(service, user, &conv, &pamh);
        if (retval == PAM_SUCCESS) {

                reply = (struct pam_response *)malloc(sizeof(struct pam_response));
                reply[0].resp = pass;
                reply[0].resp_retcode = 0;

                retval = pam_authenticate(pamh, 0);

                        //fprintf(stdout,"Coloca codigo:");
                        //scanf("%s",arreglo);


                if (retval == PAM_SUCCESS){
                       
                        
                        if((mkfifo("fifo1", mode)) < 0)
                        {
                                perror("mkfifo");
                                exit(EXIT_FAILURE);
                        }
                        if ((fd = open("fifo1", O_RDONLY)) < 0)
                        {
                                perror("open");
                                exit(EXIT_FAILURE);
                        }

                        /* Lectura y visualización de la salida del FIFO hasta EOF */
                        while((len = read(fd, buf, PIPE_BUF - 1)) > 0)
                        {
                                if(valida(arreglo,buf)==0){
                                        //printf("%s%s \n", buf,arreglo);
                                        printf("________________BIENVENIDO_____________ \n");
                                        return ( retval == PAM_SUCCESS ? 0:1 );
                                }else{
                                        //printf("Conecta USB \n");
                                        minimo = minimo +1;
                                        if(minimo>100){
                                                printf("Conecta tu USB para validar login \n");
                                                return ( retval == PAM_SUCCESS ? 0:1 );
                                        }
                                }
                        }

                        close(fd);

                }


                else{
                        fprintf(stdout, "Not Authenticated\n");
                }

                pam_end(pamh, PAM_SUCCESS);

                return ( retval == PAM_SUCCESS ? 0:1 );

        }

        return ( retval == PAM_SUCCESS ? 0:1 );
}


