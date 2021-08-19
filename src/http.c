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
    struct sockaddr * destinationSocket;
    struct addrinfo hints;
    struct addrinfo * result;

    memset( &hints, 0, sizeof( hints ));
    hints.ai_family = AF_INET; // 2
  	hints.ai_protocol = SOCK_STREAM; // 1

    /*returns one or more addrinfo structures, each of which contains an Internet address*/
    int success = getaddrinfo( url, NULL, &hints, &result );

    if( success != 0 ){ //getaddrinfo returns 0 if well succeeded
      perror("Error in getaddrinfo");
      exit(EXIT_FAILURE);
    }

    char hostname[NI_MAXHOST];

    success = getnameinfo( result->ai_addr, result->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0 );

    if( success != 0 ){
			perror("Error in getnameinfo");
			return 0;
		}

    printf("IP Address: %d\n", htonl(hostname));

    destinationSocket = result->ai_addr;

    int connection_socket = socket(result->ai_family, result->ai_protocol, 0);

    if (connection_socket == -1){
        //puts("Viiishi!");
        perror("Socket error");
        exit(EXIT_FAILURE);
    }
    ((struct sockaddr_in*) result->ai_addr)->sin_port = htons( 80 );
    int connectionResult = connect(connection_socket, destinationSocket, sizeof( struct sockaddr_in ));

    if ( connectionResult == -1){
        //puts("Não foi dessa vez!");
        perror("Connection error");
        exit(EXIT_FAILURE);
    }else{
        puts("Site connection accepted!");
    }

    char * request  = "GET http://web.ist.utl.pt/luis.tarrataca/hello.html HTTP/1.0\r\nAccept: text/plain, text/html, text/*\r\n\r\n";

  	int number_of_bytes = strlen( request ) * sizeof( char );

  	printf("[[ sendHTMLHeaderRequest ]]\n");
  	printf("\tSending HTML request message with %d bytes\n", number_of_bytes );
  	send( connection_socket, request, number_of_bytes, 0 );

		printf("\t-----HTML Request Header-----\n");
		printf("%s", request );



    close(connection_socket);

    return 0;

}
