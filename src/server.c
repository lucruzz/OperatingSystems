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
#include "../include/http2.h"
#include "../include/directories.h"

#define TIME_INTERVAL 20
#define MAX_TIME 120
#define NUM_THREADS 100

typedef struct ClientArguments_t{
    struct sockaddr_in * dest;
    int serverSocket;
    int clientSocket;
    int port;
}ClientArguments_t;

typedef struct SearchArguments_t{
    struct sockaddr_in * dest;
    char * directory;
    char * site;
    int clientSocket;
    int serverSocket;
    int port;
    int tid;
}SearchArguments_t;

Hash hashArray[TABLE_SIZE];
bool runServer;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*
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
*/

// Wrapper for seaching (embrulho)
LinkedList * searchInHashSynchronized( char * str, Hash hashArray[] ){
    pthread_mutex_lock(&lock);
    LinkedList * node = searchInHash(str, hashArray);
    pthread_mutex_unlock(&lock);
    return node;
}
// Wrapper for creating node on hash (embrulho)
LinkedList * createNodeSynchronized( char * str, int content_length, Hash hashArray[] ){
    pthread_mutex_lock(&lock);
    LinkedList * node = createNode(str, content_length, hashArray);
    pthread_mutex_unlock(&lock);
    return node;
}
// Wrapper for removing (embrulho)
LinkedList * removeNodeSynchronized( LinkedList * aux, LinkedList * node, int index, Hash hashArray [] ){
    pthread_mutex_lock(&lock);
    node = remove_Hash_Node(aux, node, index, hashArray);
    pthread_mutex_unlock(&lock);
    return node;
}

void * handleSearch( void * ptr ){

    SearchArguments_t * pt = ( SearchArguments_t * ) ptr;

    // Sinalizo a inicialização da thread
    sendString("Searching for site...", pt->clientSocket);
    // Recebo o site a ser procurado
    char * site = recvString(pt->clientSocket);
    pt->site = site;

    // Procura o site na proxy (hahstable)
    LinkedList * node = searchInHashSynchronized(pt->site, hashArray);

    if ( node == NULL ){ // Se o site não estiver na proxy (hashtable)

        sendInt(NODE_NOT_FOUND, pt->clientSocket);

        // int socket = pt->clientSocket;
        // printf("copied socket: %d | pointer copied socket: %p\n", socket, &(pt->clientSocket));//&socket);
        // buca na internet
        printf("[+] Searching for site\n");
        int content_length = http(pt->site, &(pt->clientSocket));//&socket);//, pt->clientSocket);

        // Inclui na hashtable
        // node = createNodeSynchronized(pt->site, content_length, hashArray);

        // printf("\t=== HTML file available on proxy ===\n");
        // printf("\t      %s", ctime(&node->creation_time));

        // entrega o site para o  cliente
        // sendPageToClient(node, send_to_directory, consocket);


    }else{

        // entrega o site para o cliente
        printf("[+] Page on proxy! Sending to client!\n");
        // sendPageToClient(node, send_to_directory, consocket);

        free(pt->site);
    }



    close(pt->clientSocket);

    return (void*) 0;

}

void * handleClientConnection( void * ptr ){

    //int* inteiro = (int*) ptr;
    bool runClient = true;
    ClientArguments_t * pt = (ClientArguments_t *) ptr;
    struct sockaddr_in dest = *(pt->dest);
    //printf("[+] Thread %d\n", *inteiro);
    printf("\t=== Proxy server receiving message from client %d ===\n", pt->clientSocket);
    //printf("----> %s <----\n", inet_ntoa(dest.sin_addr));

    while( runClient ){

        int consocket = pt->clientSocket;

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

                  //int serverSocket = pt->serverSocket;
                  pthread_t search_t[ n ];
                  char * send_to_directory = recvString(consocket);
                  SearchArguments_t searchArgs_t[ n ];

                  memset(&searchArgs_t, 0, n*sizeof(SearchArguments_t));

                  for ( int i = 0; i < n; i++ ){
                      socklen_t socksize = sizeof(struct sockaddr_in);
                      struct sockaddr_in target = *(pt->dest);
                      //printf("i1: %d\n", i);

                      searchArgs_t[ i ].clientSocket = accept(pt->serverSocket, (struct sockaddr *)&target, &socksize);
                      //printf("i2: %d | socket pointer: %p\n", i, &searchArgs_t[ i ].clientSocket);
                      // Recebe o argumento (site) enviado do cliente
                      // char * str = recvString(consocket);
                      // char * str = recvString(teste);
                      //printf("[%d] %s\n", i, str);
                      // searchArgs_t[ i ].site = str;
                      searchArgs_t[ i ].tid = i;
                      // searchArgs_t[ i ].clientSocket = consocket;
                      //searchArgs_t[ i ].directory = send_to_directory;

                      //searchArgs_t[ i ].clientSocket = teste;

                      int error_t = pthread_create( &search_t[ i ], NULL, handleSearch, &searchArgs_t[ i ] );

                      if(error_t){
                          printf("\t=== Sorry! The thread could not be created! ===\n");
                          continue;
                      }

                  }

                  void * retVal = NULL;

                  for( int j = 0; j < n; j++ ){

                      int success = pthread_join( search_t[ j ], retVal );
                      if(success != 0){
                          printf("Error on thread_id %ld!\n", search_t[ j ]);
                      }

                  }
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
              printf("\t=== Client %d disconnected! ===\n", consocket);
              printf("\t=== Server says bye bye to client %d! ===\n", consocket);
              close(consocket);
              runClient = false;
              break;

        }
    }

    return (void*) 0;

}

int serverConection( int port, struct sockaddr_in serv ){

    memset(&serv, 0, sizeof(serv));             // zero the struct before filling the fields
    serv.sin_family = AF_INET;                  // set the type of connection to TCP/IP
    serv.sin_addr.s_addr = htonl(INADDR_ANY);   // set our address to any interface
    serv.sin_port = htons( port ); // set the server port number

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // socket used to listen for incoming connections

    // bind serv information to mysocket
    bind(serverSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    // start listening, allowing a queue of up to 1 pending connection
    listen(serverSocket, 100);

    printf("\t=== Server is waiting for connections on port: %d ===\n", port );

    return serverSocket;
}

int acceptConnectionSocket( int serverSocket, struct sockaddr_in dest ){

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

                    node = removeNodeSynchronized(aux, node, i, hashArray);
                    // node = remove_Hash_Node(aux, node, i, hashArray);
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

    struct sockaddr_in serv; // socket info about our server
    int port = atoi( argv[ 1 ] );
    int serverSocket = serverConection( port, serv );

    if(serverSocket == COMMUNICATION_ERROR){
        return COMMUNICATION_ERROR;
    }

    int i = 0; // number of connected clients
    runServer = true;

    pthread_t threads[ NUM_THREADS ];

    if ( pthread_mutex_init(&lock, NULL) != 0){
      printf("Mutex init failed\n");
      return EXIT_FAILURE;
    }
    // Array to store pointers after memory allocation (for freeing memory in the end)
    ClientArguments_t * processClient[ NUM_THREADS ];

    memset(&hashArray, 0, TABLE_SIZE*sizeof(Hash));

    socklen_t socksize = sizeof(struct sockaddr_in);

    while( runServer ){

        struct sockaddr_in dest; // socket info about the machine connecting to us
        int consocket = accept(serverSocket, (struct sockaddr *)&dest, &socksize);
        //int consocket = acceptConnectionSocket( serverSocket, dest );

        if(consocket == COMMUNICATION_ERROR){

            if( !runServer ){
                break;
            }

            printf("Client connection failed!\n");
            continue;
        }else{
            printf("\t=== Incoming connection from %s ===\n", inet_ntoa(dest.sin_addr));
        }

        ClientArguments_t * pt = calloc(1, sizeof(ClientArguments_t));
        pt->clientSocket = consocket;
        pt->port = port;
        pt->serverSocket = serverSocket;
        pt->dest = &dest;
        printf("\t=== server socket: %d | client socket: %d | port: %d | destination: %s ===\n", pt->serverSocket, pt->clientSocket, pt->port, inet_ntoa(dest.sin_addr) );

        int error_t = pthread_create( &threads[ i ], NULL, handleClientConnection, pt );

        if(error_t){
            printf("\t=== Sorry! The thread could not be created! ===\n");
            free(pt);
            continue;
        }
        processClient[ i ] = pt;

        i++;

    }


    for( int j = 0; j < i; j++ ){

        void * returnValue = NULL;

        int success = pthread_join( threads[ j ], &returnValue );
        if(success != 0){
            printf("Error on thread_id %ld!\n", threads[ j ]);
        }
        free(processClient[ j ]);
        //printf(">> %p\n", returnValue);
    }

    // pthread_exit(NULL);

    pthread_mutex_destroy(&lock);
    removeHash(hashArray);
    close(serverSocket);
    return EXIT_SUCCESS;
}
