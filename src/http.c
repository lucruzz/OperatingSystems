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

// https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
// https://datatracker.ietf.org/doc/html/rfc2616#section-7.1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in */
#include <netdb.h>

#include "../include/http.h"

char * treatingURL( char * url ){

    int url_length = strlen(url);
    char * path_to_HTML = ( char * ) memchr (url, '/', url_length);
    int path_to_HTML_length = strlen(path_to_HTML);
    int length_server_url = url_length - path_to_HTML_length;

    char * server_URL = (char *) calloc( length_server_url + 1, sizeof(char));
    memcpy( server_URL, url, length_server_url );
    *(server_URL + length_server_url) = '\0';

    return server_URL;
}

struct addrinfo * getWebsiteSocket( char * url ){

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

    return result;
    //destinationSocket = result->ai_addr;

    //return destinationSocket;
}

int conncetionWebsiteSocket( char * url, struct addrinfo * result ){

    struct sockaddr * destinationSocket = result->ai_addr;

    int connection_socket = socket( result->ai_family, result->ai_protocol, 0);

    if (connection_socket == ERROR){
        perror("Socket error");
        exit(EXIT_FAILURE);
    }
    ((struct sockaddr_in*) destinationSocket)->sin_port = htons( 80 );
    int connectionResult = connect(connection_socket, destinationSocket, sizeof( struct sockaddr_in ));

    if ( connectionResult == ERROR){
        perror("Connection error");
        exit(EXIT_FAILURE);
    }else{
        puts("Site connection accepted!");
    }

    char request[1000];
    memset(request, 0, 1000*sizeof(char));
    strcat(request, "GET http://");
    strcat(request, url);
    strcat(request, " HTTP/1.0\r\nAccept: text/plain, text/html, text/*\r\n\r\n");

    //char * request  = "GET http://web.ist.utl.pt/luis.tarrataca/hello.html HTTP/1.0\r\nAccept: text/plain, text/html, text/*\r\n\r\n";

    int number_of_bytes = strlen( request );//* sizeof( char );

    puts("Sending request...");
    send( connection_socket, request, number_of_bytes, 0 );

    return connection_socket;
}

char * getHTMLinformation( char * url, int connection_socket ){

    // Adiciona a extensão txt para armazenar as informações da página
    int length_url = strlen(url);

    char * pt = (char *) memrchr(url, '/', length_url) + 1;
    int len_name_html = strlen(pt);

    char * info_file = (char *) calloc (len_name_html + 5, sizeof(char) );
    strcpy(info_file, pt);
    //info_file = (char *) memrchr(info_file, '/', length_url) + 1;
    strcat(info_file, ".txt");

    char * page_info = (char*) calloc (500, 1*sizeof( char ) );
    char info_char;
    int index = 0;

    FILE * pFile = fopen (info_file, "w");

    while( 1 ){

        int head_number_of_bytes = recv( connection_socket, &info_char, 1*sizeof(char), 0);

        *(page_info + index) = info_char;
        index += head_number_of_bytes;

        // De acordo com a documentação a request line vai ter pelo menos: HTTP/1.XX 200 OK\r\n
        // Então, quando page_info tiver tamanho maior que 10 já pode começar a verificar
        // Se já é possível ler o corpo do HTML
        if( index >= 5  && *( page_info + index - 4 ) == '\r' && *( page_info + index - 3 ) == '\n' &&
            *( page_info + index - 2 ) == '\r' && *( page_info + index - 1 ) == '\n' ){

                *( page_info + index ) = '\0';
                //fprintf (pFile, "%c", *info_char);
                break;

        }

    }
    //printf("%s\n", page_info);
    fprintf (pFile, "%s", page_info);

    fclose (pFile);
    free(page_info);

    printf("Informations about page collected!\n");

    return info_file;

}

int getHTMLlength( char * info_file ){
    FILE * pFile = fopen ( info_file, "r" );

    char linha[1000];

    while( fscanf( pFile, " %[^\n]%*c", linha ) != EOF ){
        char * substr = strstr(linha, "Content-Length: ");
        if(substr != NULL){

            int line_length = strlen(linha);
            char * length_html = ( char * ) memchr (linha, ' ', line_length) + 1;

            fclose(pFile);

            return atoi(length_html);
        }

    }

    fclose(pFile);
    return -2;
}

void getHTML( char * info_file, int len, int connection_socket ){

    int lenght_info_file = strlen(info_file);
    char * html_file = (char *) calloc (lenght_info_file, sizeof(char));
    strncpy( html_file, info_file, lenght_info_file - 4 );
    // free(info_file);

    FILE * html_page = fopen (html_file, "w");

    int plus_size = 10 - (len % 10);//acrescento o restante para alocação. Para que não dê erro de alocação no strcat.

    char * page = (char*) calloc (len + plus_size, sizeof( char ) );
    char string_from_page[10];
    int index = 0;
    memset(&string_from_page, 0, 10*sizeof(char));

    while( 1 ){
        int page_number_of_bytes = recv( connection_socket, &string_from_page, 10*sizeof(char), 0);
        index += page_number_of_bytes;
        strcat(page, string_from_page);

        fprintf (html_page, "%s", string_from_page);

        if( index % 10 != 0){
            break;
        }
        memset(&string_from_page, 0, 10*sizeof(char));
    }

    fclose(html_page);
    free(page);
    free(html_file);
}

void http( char * url ){

    // Se a url começar com http:// dará erro, porque não está sendo tratado
    // o inicio da url com protocolo

    char * server_URL = treatingURL( url );

    struct addrinfo * result = getWebsiteSocket( server_URL );
    free(server_URL);

    int connection_socket = conncetionWebsiteSocket( url, result );
    free(result);

    char * info_file = getHTMLinformation( url, connection_socket );
    int len = getHTMLlength( info_file );

    getHTML( info_file, len, connection_socket );

    free(info_file);
    close(connection_socket);

    printf("HTML page collected!\n");

}

/*
int main(int argc, char const *argv[]) {
  // search web.ist.utl.pt/luis.tarrataca/hello.html www.gnu.org/software/libc/manual/html_node/Internet-Namespace.html
    //www.gnu.org/software/libc/manual/html_node/Internet-Namespace.html

    char * url = "web.ist.utl.pt/luis.tarrataca/hello.html";
    // Se a url começar com http:// dará erro, porque não está sendo tratado
    // o inicio da url com protocolo

    char * server_URL = treatingURL( url );

    struct addrinfo * result = getWebsiteSocket( server_URL );
    free(server_URL);

    int connection_socket = conncetionWebsiteSocket( url, result );
    free(result);

    char * info_file = getHTMLinformation( url, connection_socket );
    /////////////////////////////////////////////////////////////
    int len = getHTMLlength( info_file );

    // FILE * pFile = fopen (info_file, "r");
    //
    // char linha[1000];
    // int len = 0;
    //
    // while( fscanf( pFile, " %[^\n]%*c", linha ) != EOF ){
    //
    //     char * substr = strstr(linha, "Content-Length: ");
    //
    //     if(substr != NULL){
    //         int line_length = strlen(linha);
    //         char * length_str = ( char * ) memchr (linha, ' ', line_length);
    //         len = atoi(length_str);
    //         break;
    //     }
    //
    // }
    //
    // fclose (pFile);
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    getHTML( info_file, len, connection_socket );
    // int lenght_info_file = strlen(info_file);
    // char * html_file = (char *) calloc (lenght_info_file, sizeof(char));
    // strncpy( html_file, info_file, lenght_info_file - 4 );
    free(info_file);
    //
    // FILE * html_page = fopen (html_file, "w");
    //
    // int plus_size = 10 - (len % 10);//acrescento o restante para alocação. Para que não dê erro de alocação no strcat.
    // int index = 0;
    //
    // char * page = (char*) calloc ( len + plus_size, sizeof( char ) );
    // char string_from_page[10];
    //
    // memset( &string_from_page, 0, 10*sizeof(char) );
    //
    // while( 1 ){
    //     int page_number_of_bytes = recv( connection_socket, &string_from_page, 10*sizeof(char), 0);
    //     index += page_number_of_bytes;
    //     strcat(page, string_from_page);
    //
    //     fprintf (html_page, "%s", string_from_page);
    //
    //     if( index % 10 != 0){
    //         break;
    //     }
    //     memset(&string_from_page, 0, 10*sizeof(char));
    // }
    //
    // fclose(html_page);
    //
    // free(page);
    // free(html_file);
    /////////////////////////////////////////////////////////////

    close(connection_socket);

    printf("HTML page collected!\n");
    return 0;
}
*/
