/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in

#include "../include/http2.h"
#include "../include/directories.h"
#include "../include/communication.h"

char * getWebsiteServer( char * url ){

    // A URL inicia assim: http://web.ist.utl.pt/luis.tarrataca/hello.html
    // Pego o tamanho da URL
    int url_length = strlen(url);

    // Ponteiro para depois do protocolo HTTP (depois da última / - http://)
    char * url_without_http = ( char * ) memchr (url, '/', url_length) + 2;

    // Pego o tamanho da string sem o http:// -> web.ist.utl.pt/luis.tarrataca/hello.html
    int url_without_http_legth = strlen(url_without_http);

    // Pego a segunda ocorrência do caracter / -> /luis.tarrataca/hello.html
    char * url_remainder = ( char * ) memchr (url_without_http, '/', url_without_http_legth);

    // Armazeno o tamanho do restante da minha string ( /luis.tarrataca/hello.html )
    int url_remainder_length = strlen(url_remainder);

    // Subtraio o tamanho restante da string ( /luis.tarrataca/hello.html )
    // do que eu tenho sem o http:// ( web.ist.utl.pt/luis.tarrataca/hello.html )
    int length_server_url = url_without_http_legth - url_remainder_length;

    // Agora eu tenho o tamanho exato que é necessário alocar para a string ( web.ist.utl.pt )
    // Faço a inclusão de mais uma posição para alocação para o caracter '\0' (fim de string)
    char * server_url = (char *) calloc( length_server_url + 1, sizeof(char));

    // Escrevo a parte desejada na string alocada
    memcpy( server_url, url_without_http, length_server_url );
    // Incluo o '\0' para indicar fim de string
    *(server_url + length_server_url) = '\0';

    return server_url;
}

struct addrinfo * getWebsiteSocket( char * url ){

    struct sockaddr_in server_add; // structure for handling internet addresses
    struct sockaddr * destinationSocket;
    struct addrinfo hints;
    struct addrinfo * result;

    memset( &hints, 0, sizeof( hints ));
    hints.ai_family = AF_INET; // 2
    hints.ai_protocol = SOCK_STREAM; // 1

    // returns one or more addrinfo structures, each of which contains an Internet address
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

    printf("\tIP Address: %s\n", hostname);

    return result;
}

int conncetionWebsiteSocket( char * url, struct addrinfo * result ){

    struct sockaddr * destinationSocket = result->ai_addr;

    int connection_socket = socket( result->ai_family, result->ai_protocol, 0);
    //printf("%d\n", connection_socket);
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
        puts("\tSite connection accepted!");
    }

    char request[HTTP_REQUEST_SIZE];
    memset(request, 0, HTTP_REQUEST_SIZE*sizeof(char));
    strcat(request, "GET ");
    strcat(request, url);
    strcat(request, " HTTP/1.0\r\nAccept: text/plain, text/html, text/*\r\n\r\n");

    //char * request  = "GET http://web.ist.utl.pt/luis.tarrataca/hello.html HTTP/1.0\r\nAccept: text/plain, text/html, text/*\r\n\r\n";

    int number_of_bytes = strlen( request );//* sizeof( char );

    puts("\tSending request...");
    send( connection_socket, request, number_of_bytes, 0 );

    return connection_socket;

}

char * getHTMLinformation( char * url, int connection_socket ){

    // Adiciona a extensão txt para armazenar as informações da página
    int length_url = strlen(url);

    char * pt = (char *) memrchr(url, '/', length_url) + 1;
    int len_name_html = strlen(pt);

    char * info_file = (char *) calloc (len_name_html + 6, sizeof(char) );
    strcpy(info_file, pt);
    //info_file = (char *) memrchr(info_file, '/', length_url) + 1;
    strcat(info_file, ".txt");

    char * page_info = (char*) calloc (LENGTH_STR_INFO_HTML, sizeof( char ) );
    char info_char;
    int index = 0;

    // Configura string para criar arquivo dentro do diretório correto
    char * path_to_info_file = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
    strcpy( path_to_info_file, INFO_PAGES_DIRECTORY );
    strcat( path_to_info_file, "/" );
    strcat( path_to_info_file, info_file );

    FILE * pFile = fopen (path_to_info_file, "w");

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
    free(path_to_info_file);

    printf("\tInformations about page collected!\n");

    return info_file;

}

// Depois de coletar a informação eu não preciso continuar armazenando essas informações
// Então, o arquivo de informação da página pode ser deletado
int getHTMLlength( char * info_file ){

    // Configura string para criar arquivo dentro do diretório correto
    char * path_to_info_file = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
    strcpy( path_to_info_file, INFO_PAGES_DIRECTORY );
    strcat( path_to_info_file, "/" );
    strcat( path_to_info_file, info_file );

    FILE * pFile = fopen ( path_to_info_file, "r" );

    char linha[HTML_INFO_STR_SIZE];

    while( fscanf( pFile, " %[^\n]%*c", linha ) != EOF ){
        char * substr = strstr(linha, "Content-Length: ");
        if(substr != NULL){

            int line_length = strlen(linha);
            char * length_html = ( char * ) memchr (linha, ' ', line_length) + 1;

            fclose(pFile);
            free(path_to_info_file);

            // removeFile(INFO_PAGES_DIRECTORY, info_file);

            return atoi(length_html);
        }

    }

    fclose(pFile);
    free(path_to_info_file);

    // removeFile(INFO_PAGES_DIRECTORY, info_file);

    return CONTENT_LENGTH_NOT_FOUND;
}

void getHTML( char * info_file, int len, int connection_socket, int clientSocket ){

    int lenght_info_file = strlen(info_file);
    char * html_file = (char *) calloc (lenght_info_file, sizeof(char));
    strncpy( html_file, info_file, lenght_info_file - 4 );
    // free(info_file);

    // Envio o nome da página para o cliente
    sendString(html_file, clientSocket);

    // Configura string para criar arquivo dentro do diretório correto
    char * path_to_html_file = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
    strcpy( path_to_html_file, PROXY_DIRECTORY );
    strcat( path_to_html_file, "/" );
    strcat( path_to_html_file, html_file );



    FILE * html_page = fopen (path_to_html_file, "w");

    //printf("\tLENGTH HTML PAGE: %d\n", len);

    //int plus_size = len - (len % N_BYTES_TO_RECV);//acrescento o restante para alocação. Para que não dê erro de alocação no strcat.

    // char string_from_page[N_BYTES_TO_RECV];
    // char * string_from_page = (char*) calloc ( N_BYTES_TO_RECV + 1, sizeof( char ) );
    char * string_from_page = (char*) calloc ( N_BYTES_TO_RECV, sizeof( char ) );
    int index = 0;
    // memset(&string_from_page, 0, sizeof(char));
    //memset(string_from_page, 0, N_BYTES_TO_RECV*sizeof(char) + 1);

    // int k = 0;
    while( 1 ){
        // int page_number_of_bytes = recv( connection_socket, &string_from_page, N_BYTES_TO_RECV*sizeof(char), 0);
        int page_number_of_bytes = recv( connection_socket, string_from_page, N_BYTES_TO_RECV*sizeof(char), 0);
        // int page_number_of_bytes = recvString2(string_from_page, connection_socket);
        //send( clientSocket, string_from_page, N_BYTES_TO_RECV*sizeof(char), 0);
        //printf("%s\n", string_from_page);
        //printf("---> strlen(): %d | bytes reads: %d<---\n", (int)strlen(string_from_page), page_number_of_bytes);
        // send( clientSocket, "oi", 2*sizeof(char), 0);
        // sendString2( string_from_page, page_number_of_bytes, clientSocket );
        printf("bytes: %d\n", page_number_of_bytes);

        index += page_number_of_bytes;
        //fprintf (html_page, "%s", string_from_page);
        fputs(string_from_page, html_page);

        if( index % len == 0){
            printf("bytes reads> %d\n", index);
            break;
        }
        // memset(&string_from_page, 0, sizeof(char));
        // memset(string_from_page, 0, N_BYTES_TO_RECV*sizeof(char) + 1);
        memset(string_from_page, 0, N_BYTES_TO_RECV*sizeof(char) );
    }

    fclose(html_page);
    free(path_to_html_file);
    free(html_file);
    free(string_from_page);

}

// Essa função é para ser usada quando a página não tiver a informção "content-length"
// Ela vai pegar a página normalmente e retornar a quantidade de bytes lidos.
int getHTML_With_No_Length_Found( char * info_file, int connection_socket ){

    int lenght_info_file = strlen(info_file);
    char * html_file = (char *) calloc (lenght_info_file, sizeof(char));
    strncpy( html_file, info_file, lenght_info_file - 4 );

    // Configura string para criar arquivo dentro do diretório correto
    char * path_to_html_file = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
    strcpy( path_to_html_file, PROXY_DIRECTORY );
    strcat( path_to_html_file, "/" );
    strcat( path_to_html_file, html_file );

    FILE * html_page = fopen (path_to_html_file, "w");

    char string_from_page[N_BYTES_TO_RECV];
    int bytes_reads = 0;
    memset(&string_from_page, 0, N_BYTES_TO_RECV*sizeof(char));

    while( 1 ){

        int page_number_of_bytes = recv( connection_socket, &string_from_page, N_BYTES_TO_RECV*sizeof(char), 0);
        bytes_reads += page_number_of_bytes;

        //fprintf (html_page, "%s", string_from_page);
        fputs(string_from_page, html_page);

        if( page_number_of_bytes == 0){
            break;
        }

        memset(&string_from_page, 0, N_BYTES_TO_RECV*sizeof(char));

    }

    fclose(html_page);
    free(html_file);
    free(path_to_html_file);

    return bytes_reads; // retorna  o número de bytes lidos
}

bool checkURL( char * url ){

    char * p = strstr(url, HTTP_STR);

    if( p == NULL ){
        printf("Please, make sure you are searching for website using HTTP protocol!\n");
        printf("If the website you are searching for uses HTTP protocol, please include 'http://'\n");
        return false;
    }

    return true;

}


int http( char * url, int * clientSocket ){

    if( !checkURL( url ) ){ // Se a url não estiver de acordo com os parâmetros do programa
        return ERROR_URL_NOT_HTTP; // retorna que deu ruim
    }

    char * serverIP = getWebsiteServer( url );

    struct addrinfo * result = getWebsiteSocket( serverIP );
    free(serverIP);
    int connection_socket = conncetionWebsiteSocket( url, result );
    free(result);

    char * info_file = getHTMLinformation( url, connection_socket );
    int len = getHTMLlength( info_file );

    // envio para o cliente o número de bytes da página solicitada
    sendInt(len, *clientSocket);

    if(len == CONTENT_LENGTH_NOT_FOUND){
        printf("\t=== No page content-length found! ===\n");
        len = getHTML_With_No_Length_Found( info_file, connection_socket );
    }else{
        printf("\t=== Page content-length found: %d ===\n", len);
      	getHTML( info_file, len, connection_socket, *clientSocket );
    }

    free(info_file);
    close(connection_socket);

    printf("\t=== HTML page collected! ===\n");

    return len;

}


/* int http( char * url )
{

    if( !checkURL( url ) ){ // Se a url não estiver de acordo com os parâmetros do programa
        return ERROR_URL_NOT_HTTP; // retorna que deu ruim
    }

    char * serverIP = getWebsiteServer( url );

    struct addrinfo * result = getWebsiteSocket( serverIP );
    free(serverIP);

    int connection_socket = conncetionWebsiteSocket( url, result );
    free(result);

    char * info_file = getHTMLinformation( url, connection_socket );
    int len = getHTMLlength( info_file );

    if(len == CONTENT_LENGTH_NOT_FOUND){
        printf("\tNo page content-length found!\n");
        len = getHTML_With_No_Length_Found( info_file, connection_socket );
    }else{
        printf("\tPage content-length found: %d\n", len);
      	getHTML( info_file, len, connection_socket );
    }

    free(info_file);
    close(connection_socket);

    printf("\tHTML page collected!\n");

    return len;

}

*/
