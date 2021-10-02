/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "../include/communication.h"
#include "../include/hashServer.h"
#include "../include/http.h"
#include "../include/directories.h"

#define TIME_INTERVAL 20
#define MAX_TIME 120
#define NUM_THREADS 2

typedef struct ClientArguments_t{
    int socket;
    int serverSocket;
    int port;
}ClientArguments_t;

typedef struct SearchArguments_t{
    char * directory;
    char * site;
    int socket;
    int serverSocket;
    int port;
}SearchArguments_t;

Hash hashArray[TABLE_SIZE];
bool runServer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sendPageToClient( LinkedList * node, char  * send_to_directory, int consocket ){

    int content_length = node->content_length;
    char * str = node->site;

    // Envia o número de bytes da página
    sendInt(content_length, consocket);

    // esse trecho aqui pode ser generalizado, porque também é uma parte no http.c
    // aqui eu pego o nome do arquivo que foi salvo na proxy

    // REALMENTE PRECISA ALOCAR MEMÓRIA AQUI? ACHO QUE NÃO
    char * pt = memrchr(str, '/', (int)strlen(str)) + 1;
    char * file_to_send = (char *) calloc ((int)strlen(pt) + 1, sizeof(char) );
    strcpy(file_to_send, pt);

    printf("\t=== Sending %s file to client ===\n", file_to_send);

    // Envio o nome da página
    sendString(file_to_send, consocket);

    // Configura string para criar arquivo dentro do diretório correto
    char * path_to_html_file_on_proxy = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
    strcpy( path_to_html_file_on_proxy, PROXY_DIRECTORY );
    strcat( path_to_html_file_on_proxy, "/" );
    strcat( path_to_html_file_on_proxy, file_to_send );


    char * string_line_from_file = (char *) calloc( N_BYTES_TO_SEND + 1, sizeof(char));

    FILE * p = fopen(path_to_html_file_on_proxy, "r");

    int number_of_bytes_reads = 0; // contador para content_length
    int pos = 0; // indice para armazenar o caracter

    while( 1 ){

        char c = fgetc( p );

        if( c == EOF ){
            sendString2(string_line_from_file, consocket);
            break;
        }

        if( strlen(string_line_from_file) == N_BYTES_TO_SEND){
            sendString2(string_line_from_file, consocket);
            memset(string_line_from_file, 0, N_BYTES_TO_SEND);
            pos = 0;
        }
        *(string_line_from_file + pos) = c;
        number_of_bytes_reads++;
        pos++;
    }

    printf("\t=== File %s sended to %s ===\n",  path_to_html_file_on_proxy, send_to_directory);
    printf("\t=== %d bytes reads ===\n", number_of_bytes_reads);

    fclose(p);

    free(file_to_send);
    free(path_to_html_file_on_proxy);
    free(string_line_from_file);

}

LinkedList * searchInHashSynchronized( char * str, Hash hashArray[] ){
    pthread_mutex_lock(&mutex);
    LinkedList * node = searchInHash(str, hashArray);
    pthread_mutex_unlock(&mutex);
    return node;
}

// Wrapper (embrulho)
LinkedList * createNodeSynchronized( char * str, int content_length, Hash hashArray[] ){
    pthread_mutex_lock(&mutex);
    LinkedList * node = createNode(str, content_length, hashArray);
    pthread_mutex_unlock(&mutex);
    return node;
}

void * handleSearch( void * ptr ){

    SearchArguments_t * pt = (SearchArguments_t * ) ptr;

    //int searchConSocket = searchConnectionSocket( pt->port );
    int consocket = pt->socket;
    //int serverSocket = pt->serverSocket;
    char * send_to_directory = pt->directory;
    char * str = pt->site;

    printf("search Socket: %d | directory: %s | site: %s\n", consocket, pt->directory, pt->site);
    //int consocket = acceptSearchConnectionSocket( serverSocket );

    // // Recebe o argumento (site) enviado do cliente
    // char * str = recvString(consocket);

    // pthread_mutex_lock(&mutex);
    // LinkedList * node = searchInHash(str, hashArray);
    // pthread_mutex_unlock(&mutex);
    LinkedList * node = searchInHashSynchronized(str, hashArray);
    // printf("SOCKET> %d\nDIRECTORY> %s\nURL> %s\nNODE> %p\n", consocket, send_to_directory, str, node);


    if( node == NULL ){
        // busca na internet

        printf("\t=== Searching for site ===\n");

        // pthread_mutex_lock(&mutex);

        int content_length = http(str);
        // pthread_mutex_unlock(&mutex);

        // pthread_mutex_lock(&mutex);
        // Inclui na hash
        // node = createNode(str, content_length, hashArray);
        // pthread_mutex_unlock(&mutex);
        node = createNodeSynchronized(str, content_length, hashArray);

        printf("\t=== HTML file available on proxy ===\n");
        printf("\t      %s", ctime(&node->creation_time));

//////////////////////////////////////////////////////////////////
        //pthread_mutex_lock(&mutex);
        // entrega o site para o cliente
        // sendPageToClient(node, send_to_directory, consocket);
        //pthread_mutex_unlock(&mutex);
//////////////////////////////////////////////////////////////////

        // Envia o número de bytes da página
        sendInt(content_length, consocket);

        // esse trecho aqui pode ser generalizado, porque também é uma parte no http.c
        // aqui eu pego o nome do arquivo que foi salvo na proxy

        // REALMENTE PRECISA ALOCAR MEMÓRIA AQUI? ACHO QUE NÃO
        char * pt = memrchr(str, '/', (int)strlen(str)) + 1;
        char * file_to_send = (char *) calloc ((int)strlen(pt) + 1, sizeof(char) );
        //char file_to_send[(int)strlen(pt) + 1];
        //memset(file_to_send, 0, (int)strlen(pt) + 1);
        strcpy(file_to_send, pt);

        printf("\t=== Sending %s file to client ===\n", file_to_send);

        // Envio o nome da página
        //sendString(*(&file_to_send), consocket);
        sendString(file_to_send, consocket);
/*
        // Configura string para criar arquivo dentro do diretório correto
        //char * path_to_html_file_on_proxy = ( char * ) calloc( LENGTH_DIR_PATH, sizeof(char) );
        char path_to_html_file_on_proxy[ LENGTH_DIR_PATH ];
        memset( path_to_html_file_on_proxy, 0, LENGTH_DIR_PATH );
        strcpy( path_to_html_file_on_proxy, PROXY_DIRECTORY );
        strcat( path_to_html_file_on_proxy, "/" );
        strcat( path_to_html_file_on_proxy, file_to_send );

        //printf(">>%s", path_to_html_file_on_proxy);

        // char * string_line_from_file = (char *) calloc( N_BYTES_TO_SEND + 1, sizeof(char));
        char string_line_from_file[ N_BYTES_TO_SEND + 1 ];
        memset( string_line_from_file, 0, N_BYTES_TO_SEND + 1);
        FILE * p = fopen(path_to_html_file_on_proxy, "r");

        int number_of_bytes_reads = 0; // contador para content_length
        int pos = 0; // indice para armazenar o caracter

        while( 1 ){

            char c = fgetc( p );

            if( c == EOF ){
                sendString2(*(&string_line_from_file), consocket);
                break;
            }

            if( strlen(string_line_from_file) == N_BYTES_TO_SEND){
                sendString2(*(&string_line_from_file), consocket);
                memset(string_line_from_file, 0, N_BYTES_TO_SEND + 1);
                pos = 0;
            }
            *(string_line_from_file + pos) = c;
            number_of_bytes_reads++;
            pos++;
        }

        printf("\t=== File %s sended to %s ===\n",  path_to_html_file_on_proxy, send_to_directory);
        printf("\t=== %d bytes reads ===\n", number_of_bytes_reads);

        fclose(p);

        // free(file_to_send);
        // free(path_to_html_file_on_proxy);
        //free(string_line_from_file);
*/
    }else{
        // entrega o site para o cliente
        printf("\t=== Page on proxy! Sending to client! ===\n");

        sendPageToClient(node, send_to_directory, consocket);

        free(str);
    }

    //close(consocket);

    return (void*) 0;

}


void * handleClientConnection( void * ptr ){

    int* inteiro = (int*) ptr;
    bool runClient = true;
    ClientArguments_t * pt = (ClientArguments_t *) ptr;

    printf("\t=== Proxy server receiving message from client %d ===\n", *inteiro);

    while( runClient ){

        int consocket = pt->socket;

        //Recebe o ID do comando a ser processado
        int commandReceived_id = 0;
        commandReceived_id = recvInt(consocket);

        switch (commandReceived_id) {

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

              if( n > 0 ){

                  pthread_t search_t[ n ];
                  char * send_to_directory = recvString(consocket);
                  SearchArguments_t searchArgs_t[ n ];

                  for ( int i = 0; i < n; i++ ){

                      //searchArgs_t[ i ].serverSocket = pt->serverSocket;//searchConSocket;
                      searchArgs_t[ i ].directory = send_to_directory;

                      // Recebe o argumento (site) enviado do cliente
                      char * str = recvString(consocket);
                      searchArgs_t[ i ].site = str;

                      //int searchConSocket = acceptSearchConnectionSocket(serverSocket);
                      struct sockaddr_in dest;
                      socklen_t socksize = sizeof(struct sockaddr_in);
                      int searchConSocket = accept(pt->serverSocket, (struct sockaddr *)&dest, &socksize);
                      searchArgs_t[ i ].socket = searchConSocket;

                      // cria uma thread para processar a busca
                      int error_t = pthread_create( &search_t[ i ], NULL, handleSearch, &searchArgs_t[ i ] );

                      if(error_t){
                          printf("\t=== [search] Error: Thread could not be created! ===\n");
                      }


                  }

                  void * returnValue = NULL;

                  for( int j = 0; j < n; j++ ){

                      int success = pthread_join( search_t[ j ], returnValue );
                      if(success != 0){
                          printf("Error on thread_id %ld!\n", search_t[ j ]);
                      }

                      close(searchArgs_t[ j ].socket);

                  }

                  /////////////////////////////////////////////////////
                  // for( int i = 0; i < n; i++ ){
                  //     // Recebe o argumento (site) enviado do cliente
                  //     char * str = recvString(consocket);
                  //
                  //     LinkedList * node = searchInHash(str, hashArray);
                  //
                  //     if( node == NULL ){
                  //         // busca na internet
                  //
                  //         printf("\t=== Searching for site ===\n");
                  //
                  //         int content_length = http(str);
                  //
                  //         // sem_wait(&sem1);
                  //         pthread_mutex_lock(&mutex);
                  //         // Inclui na hash
                  //         node = createNode(str, content_length, hashArray);
                  //         // sem_post(&sem1);
                  //         pthread_mutex_unlock(&mutex);
                  //
                  //         printf("\t=== HTML file available on proxy ===\n");
                  //         printf("\t      %s", ctime(&node->creation_time));
                  //
                  //         // entrega o site para o cliente
                  //         sendPageToClient(node, send_to_directory, consocket);
                  //
                  //     }else{
                  //         // entrega o site para o cliente
                  //         printf("\t=== Page on proxy! Sending to client! ===\n");
                  //
                  //         sendPageToClient(node, send_to_directory, consocket);
                  //
                  //         free(str);
                  //     }
                  //
                  // }
                  /////////////////////////////////////////////////////
                  free(send_to_directory);
              }

              break;

          case HISTORY_ID:
              printf("[HISTORY COMMAND]\n");
              break;

          case COMMAND_ID_NOT_FOUND:
              printf("[COMMAND NOT FOUND]\n");
              break;

          case EXIT_ID:
              printf("[EXIT COMMAND]\n");
              close(consocket);
              printf("\t=== Client %d disconnected! ===\n", *inteiro);
              printf("\t=== Server says bye bye to client %d! ===\n", *inteiro);
              runClient = false;
              break;

        }
    }

    return (void*) 0;

}

int serverConection(char * argv[]){

    struct sockaddr_in serv; // socket info about our server
    int serverSocket;        // socket used to listen for incoming connections

    memset(&serv, 0, sizeof(serv));             // zero the struct before filling the fields
    serv.sin_family = AF_INET;                  // set the type of connection to TCP/IP
    serv.sin_addr.s_addr = htonl(INADDR_ANY);   // set our address to any interface
    serv.sin_port = htons( atoi( argv[ 1 ] ) ); // set the server port number

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // bind serv information to mysocket
    bind(serverSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    // start listening, allowing a queue of up to 1 pending connection
    listen(serverSocket, 1);

    printf("\t=== Server is waiting for connections on port: %s ===\n", argv[ 1 ] );

    return serverSocket;
}

int acceptConnectionSocket(int serverSocket){

    struct sockaddr_in dest; // socket info about the machine connecting to us
    socklen_t socksize = sizeof(struct sockaddr_in);

    int consocket = accept(serverSocket, (struct sockaddr *)&dest, &socksize);

    if( consocket != COMMUNICATION_ERROR){
        printf("\t=== Incoming connection from %s ===\n", inet_ntoa(dest.sin_addr));
    }

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

void  closeProxy(int sig){
     char  c;

     signal(sig, SIG_IGN);
     printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y')
          //exit(0);
          runServer = false;
     else
          signal(SIGINT, closeProxy);
     getchar(); // Get new line character
}

int main(int argc, char *argv[]){

    if( argc != 2 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    signal( SIGALRM, verifyHashtable ); // Register signal handler
    //signal(SIGINT, closeProxy); // Signal to close/exit Proxy server

    alarm( TIME_INTERVAL ); // Scheduled alarm after 20 seconds

    int serverSocket = serverConection(argv);

    if(serverSocket == COMMUNICATION_ERROR){
        return COMMUNICATION_ERROR;
    }

    int i = 0; // number of connected clients
    runServer = true;

    pthread_t threads[ NUM_THREADS ];

    if ( pthread_mutex_init(&mutex, NULL) != 0){
      printf("Mutex init failed\n");
      return EXIT_FAILURE;
    }
    // Array to store pointers after memory allocation (for freeing memory in the end)
    ClientArguments_t * processClient[ NUM_THREADS ];

    memset(&hashArray, 0, TABLE_SIZE*sizeof(Hash));

    while( runServer ){

        int consocket = acceptConnectionSocket(serverSocket);

        if(consocket == COMMUNICATION_ERROR){

            if( !runServer ){
                break;
            }

            printf("Client connection failed!\n");
            continue;
        }

        ClientArguments_t * pt = calloc(1, sizeof(ClientArguments_t));
        pt->socket = consocket;
        pt->port = atoi( argv[ 1 ] );
        pt->serverSocket = serverSocket;

        int error_t = pthread_create( &threads[ i ], NULL, handleClientConnection, pt );

        if(error_t){
            printf("\t=== Sorry! The thread could not be created!\n ===");
            free(pt);
            continue;
        }
        processClient[ i ] = pt;

        i++;

    }

    void * returnValue = NULL;

    for( int j = 0; j < i; j++ ){

        int success = pthread_join( threads[ j ], returnValue );
        if(success != 0){
            printf("Error on thread_id %ld!\n", threads[ j ]);
        }
        free(processClient[ j ]);
        //printf(">> %p\n", returnValue);
    }

    // pthread_exit(NULL);

    pthread_mutex_destroy(&mutex);
    removeHash(hashArray);
    close(serverSocket);
    return EXIT_SUCCESS;
}
