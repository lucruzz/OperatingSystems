/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>

#include "../include/communication.h"
#include "../include/hashServer.h"
#include "../include/http.h"
#include "../include/directories.h"

#define TIME_INTERVAL 20
#define MAX_TIME 120

Hash hashArray[TABLE_SIZE];

void executeCommand(int command_id, Hash hashArray[], int * run, int consocket){

      switch (command_id) {

        case LIST_ID:
            printf("[LIST COMMAND]\n");
            //printHash(hashArray);

            int i = 0;

            while( i < TABLE_SIZE ){

                if(hashArray[i].begin == NULL){

                    sendInt(0, consocket);

                }else{

                    LinkedList * node = hashArray[i].begin;
                    int n = hashArray[i].n_elements;

                    sendInt(n, consocket);

                    for ( int j = 1; j <= n; j++ ){
                        sendString(node->site, consocket);
                        node = node->next;
                    }

                }
                i++;

            }

            break;

        case SEARCH_ID:
            printf("[SEARCH COMMAND]\n");

            // Recebe o número de argumentos (sites) a serem processados
            int n = recvInt(consocket);
            char * send_to_directory = recvString(consocket);

            if(n){
                for( int i = 0; i < n; i++ ){
                    // Recebe o argumento (site) enviado do cliente
                    char * str = recvString(consocket);

                    LinkedList * node = searchInHash(str, hashArray);

                    if( node == NULL ){
                        // busca na internet
                        int content_length = http(str);

                        // Inclui na hash
                        node = createNode(str, content_length, hashArray);

                        // entrega o site para o cliente

                        // Envia o número de bytes da página
                        sendInt(content_length, consocket);

                        // esse trecho aqui pode ser generalizado, porque também é uma parte no http.c
                        // aqui eu pego o nome do arquivo que foi salvo na proxy
                        char * pt = memrchr(str, '/', strlen(str)) + 1;
                        char * file_to_send = (char *) calloc (strlen(pt) + 1, sizeof(char) );
                        strcpy(file_to_send, pt);

                        printf("Sending %s file to client...\n", pt);

                        // Envio o nome da página
                        sendString(file_to_send, consocket);

                        // Configura string para criar arquivo dentro do diretório correto
                        char * path_to_html_file_on_proxy = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
                        strcpy( path_to_html_file_on_proxy, PROXY_DIRECTORY );
                        strcat( path_to_html_file_on_proxy, "/" );
                        strcat( path_to_html_file_on_proxy, file_to_send );

                        int number_of_bytes_reads = 0;
                        int send_n_bytes = N_BYTES_TO_SEND;
                        char string_line_from_file[send_n_bytes];

                        FILE * p = fopen(path_to_html_file_on_proxy, "r");
                        memset(&string_line_from_file, 0, send_n_bytes*sizeof(char));

                        while ( number_of_bytes_reads != content_length ){

                            fgets(string_line_from_file, send_n_bytes, p);
                            number_of_bytes_reads += strlen(string_line_from_file);
                            sendString(string_line_from_file, consocket);
                            memset(&string_line_from_file, 0, send_n_bytes*sizeof(char));

                        }
                        printf("\nFile %s sended to %s\n",  path_to_html_file_on_proxy, send_to_directory);
                        fclose(p);
                        free(file_to_send);
                        free(path_to_html_file_on_proxy);

                    }else{
                        // entrega o site para o cliente
                        puts("Page on proxy!");
                        free(str);
                    }

                }
            }

            free(send_to_directory);

            break;

        case HISTORY_ID:
            printf("[HISTORY COMMAND]\n");
            break;

        case COMMAND_ID_NOT_FOUND:
            printf("[COMMAND NOT FOUND]\n");
            break;

        case EXIT_ID:
            printf("[EXIT COMMAND]\n");
            printf("Server says bye bye!\n");
            *run = FALSE;
            break;

      }

}

int serverConection(char * argv[]){

    struct sockaddr_in serv; /* socket info about our server */
    int serverSocket;        /* socket used to listen for incoming connections */

    memset(&serv, 0, sizeof(serv));             /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                  /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY);   /* set our address to any interface */
    serv.sin_port = htons( atoi( argv[ 1 ] ) ); /* set the server port number */

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* bind serv information to mysocket */
    bind(serverSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(serverSocket, 1);

    printf("Server is waiting for connections on port:%s\n", argv[ 1 ] );

    return serverSocket;
}

int connectionSocket(int serverSocket){

    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    socklen_t socksize = sizeof(struct sockaddr_in);

    int consocket = accept(serverSocket, (struct sockaddr *)&dest, &socksize);

    printf("Incoming connection from %s - sending welcome\n", inet_ntoa(dest.sin_addr));

    return consocket;
}

void verifyHashtable( int signum ){

  int i = 0;

  while( i < TABLE_SIZE ){

    if( hashArray[i].begin != NULL ){

        LinkedList * node = hashArray[i].begin;
        LinkedList * aux = node;

        while( node != NULL ){

            time_t now = time( NULL );
            double timeDifference = difftime( now, node->creation_time );

            if(timeDifference > MAX_TIME){

                char * pt = memrchr(node->site, '/', strlen(node->site)) + 1;
                char * file = (char *) calloc (strlen(pt) + 1, sizeof(char) );
                char * info_file = (char *) calloc (strlen(pt) + 6, sizeof(char) );
                strcpy(file, pt);
                strcpy(info_file, pt);
                strcat(info_file, ".txt");

                node = remove_Hash_Node(aux, node, i, hashArray);
                char * timeString = ctime( &now );

                removeFile( PROXY_DIRECTORY, file);
                removeFile( INFO_PAGES_DIRECTORY, info_file);

                printf("\t[file %s removed from proxy!]\n", file);
                printf("Current time: %s\n", timeString);

              	//printf("Time difference: %f\n", timeDifference);

                free(file);
                free(info_file);

                continue;
            }
            aux = node;
            node = node->next;
        }

    }
    i++;

  }

	alarm( TIME_INTERVAL );

}

int main(int argc, char *argv[]){

    if( argc != 2 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    signal( SIGALRM, verifyHashtable ); // Register signal handler

    alarm( TIME_INTERVAL ); // Scheduled alarm after 20 seconds

    int run = TRUE;
    int serverSocket = serverConection(argv);
    int consocket = connectionSocket(serverSocket);

    memset(&hashArray, 0, TABLE_SIZE*sizeof(Hash));

    // Cria o diretório com as informações da página
    // if ( !makeDirectory(INFO_PAGES_DIRECTORY) ) { return 0; }

    // Cria o diretório da proxy para armazenar as páginas que o cliente poderá baixar
    // if ( !makeDirectory(PROXY_DIRECTORY) ) { return 0; }

    while(run){

        //Recebe o ID do comando a ser processado
        int commandReceived_id = 0;
        commandReceived_id = recvInt(consocket);

        // Executa o comando de acordo com o ID do comando recebido
        executeCommand(commandReceived_id, hashArray, &run, consocket);

    }

    removeHash(hashArray);

    // remove os diretórios criados
    // removeDirectory(INFO_PAGES_DIRECTORY);
    // removeDirectory(PROXY_DIRECTORY);

    close(consocket);
    close(serverSocket);
    return EXIT_SUCCESS;
}
