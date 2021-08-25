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
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#include "../include/communication.h"
#include "../include/hashServer.h"
#include "../include/http.h"

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

            if(n){
                for( int i = 0; i < n; i++ ){
                    // Recebe o argumento (site) enviado do cliente
                    char * str = recvString(consocket);

                    LinkedList * node = searchInHash(str, hashArray);
                    if( node == NULL ){
                      // busca na internet
                      http(str);

                      // Inclui na hash
                      createNode(str, hashArray);
                    }else{
                      // entrega o site para o cliente
                      free(str);
                    }

                }
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

int main(int argc, char *argv[]){

    if( argc != 2 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    int run = TRUE;
    int serverSocket = serverConection(argv);
    int consocket = connectionSocket(serverSocket);
    Hash hashArray[TABLE_SIZE];
    memset(&hashArray, 0, TABLE_SIZE*sizeof(Hash));


    while(run){

        //Recebe o ID do comando a ser processado
        int commandReceived_id = 0;
        commandReceived_id = recvInt(consocket);

        // Executa o comando de acordo com o ID do comando recebido
        executeCommand(commandReceived_id, hashArray, &run, consocket);

    }

    removeHash(hashArray);
    close(consocket);
    close(serverSocket);
    return EXIT_SUCCESS;
}
