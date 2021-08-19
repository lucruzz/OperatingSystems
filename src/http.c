/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
// https://www.gta.ufrj.br/ensino/eel878/sockets/index.html
// https://www.gnu.org/software/libc/manual/html_node/index.html#SEC_Contents
// https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in */
#include <netdb.h>

int main(int argc, char const *argv[]) {

    char * url = "web.ist.utl.pt";///luis.tarrataca/hello.html";

    struct sockaddr_in server_add; /*structure for handling internet addresses*/
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int connection_socket;

    memset( &hints, 0, sizeof( hints ));
    hints.ai_family = AF_INET; // 2
  	hints.ai_protocol = SOCK_STREAM; // 1

    int success = getaddrinfo( url, NULL, &hints, &result );

    if( !!success ){ //getaddrinfo returns 0 if well succeeded
      perror("Error in getaddrinfo");
      exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {

        connection_socket = socket(rp->ai_family, rp->ai_protocol, 0);

        if (connection_socket == -1){
            //puts("Viiishi!");
            perror("Socket error");
            continue;
        }
        ((struct sockaddr_in*) rp->ai_addr)->sin_port = htons(80);
        int connectionResult = connect(connection_socket, rp->ai_addr, sizeof( struct sockaddr_in ));

        if ( connectionResult == -1){
            //puts("Não foi dessa vez!");
            perror("Connection error");
            continue;
        }
        //puts("Tá indo!!!! Amém, Jesus!");

        close(connection_socket);

     }

    return 0;

}
