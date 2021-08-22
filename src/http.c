/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
// https://www.gta.ufrj.br/ensino/eel878/sockets/index.html
// https://www.gnu.org/software/libc/manual/html_node/index.html#SEC_Contents
// https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

// https://stackoverflow.com/questions/22077802/simple-c-example-of-doing-an-http-post-and-consuming-the-response
// https://www.quora.com/What-exactly-is-r-in-the-C-language
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in */
#include <netdb.h>

int main(int argc, char const *argv[]) {

    //char * url = "web.ist.utl.pt";///luis.tarrataca/hello.html";
    char * url = "www.google.com";

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

    printf("IP Address: %s\n", hostname);

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

    //char * request  = "GET http://www.google.com/preferences HTTP/1.0\r\nAccept: text/plain, text/html, text/*\r\n\r\n";
  	int number_of_bytes = strlen( request );// * sizeof( char );

  	puts("Sending request...");
  	send( connection_socket, request, number_of_bytes, 0 );

    char * page_info = (char*) calloc (500, sizeof( char ) );
    char * info_char;
    int index = 0;

    while( 1 ){

        int head_number_of_bytes = recv( connection_socket, info_char, 1*sizeof(char), 0);

        index += head_number_of_bytes;

        *( page_info + index - 1 ) = *info_char;

        if( page_info[index - 4] == '\r' && page_info[index - 3] == '\n' &&
            page_info[index - 2] == '\r' && page_info[index - 1] == '\n' ){
            break;
        }

    }
    puts("-----------------------------------------------");
    printf("%s\n", page_info);

    char * page = (char*) calloc (500, sizeof( char ) );
    char string_from_page[10];
    index = 0;
    memset(&string_from_page, 0, 10*sizeof(string_from_page));

    while( 1 ){
        int page_number_of_bytes = recv( connection_socket, &string_from_page, 10*sizeof(char), 0);
        index += page_number_of_bytes;
        strcat(page, string_from_page);

        if(index%10 != 0){
          break;
        }
        memset(&string_from_page, 0, 10*sizeof(string_from_page));
    }

    puts("-----------------------------------------------");

    printf("%s\n", page);

    close(connection_socket);
    free(result);
    free(page);
    free(page_info);

    return 0;

}
