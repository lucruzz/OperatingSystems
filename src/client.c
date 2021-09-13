/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "../include/history.h"
#include "../include/argsList.h"
#include "../include/directories.h"
#include "../include/communication.h"
#include "../include/hashServer.h" // TABLE_SIZE

#define MAX_LINE_READ_COMMAND_SIZE 1000
#define MAX_LINE_PATH_SHARED_FOLDER 20

typedef struct ClientInformation{
    int socket;
    int port;
    char * shared_directory;
}ClientInformation;

List * determineArguments(char * commandArgs){

    int n_arguments = 0;
    List * commandList = createList();

    commandArgs = strtok(NULL, " ");

    while( commandArgs != NULL ){
        ++n_arguments;
        insertArg(commandArgs, commandList);
        commandArgs = strtok(NULL, " ");
    }

    return commandList;
}

// Busca n sites
void search(char *command, char *shared_directory, int mysocket){

    puts("> Search");

    List * commandList = determineArguments(command);

    int n = commandList->n_elements;
    // Envia o número de argumentos (sites)
    sendInt(n, mysocket);

    // Envia o diretório compartilhado a que deseja receber os dados
    sendString(shared_directory, mysocket);

    if(n){
        Node * aux = commandList->begin;

        // Pego o tamanho do caminho do diretório
        int length_str_shared_directory = strlen(shared_directory);

        for( int i = 0; i < n; i++ ){
            // Envia o argumento (site) para o Servidor
            sendString(aux->argument, mysocket);

            // Recebo a quantidade de bytes da página a ser recebida
            int n_bytes_to_recv = recvInt(mysocket);
            printf("\t=== Content-Length: %d ===\n", n_bytes_to_recv);

            // Recebo o nome da paǵina
            char * page_filename = recvString(mysocket);
            // Pego o tamanho do nome da página
            int length_str_page_filename = strlen(page_filename);

            int total_full_length_to_file = length_str_shared_directory + length_str_page_filename;

            // Aloco uma string para o tamanho total para o arquivo "no lado do" cliente
            char * full_path_file = ( char * ) calloc(total_full_length_to_file + 2, sizeof(char));
            strcat(full_path_file, shared_directory);
            strcat(full_path_file, "/");
            strcat(full_path_file, page_filename);

            // Abro o arquivo para armazenar a página "no lado" cliente
            FILE * p = fopen(full_path_file, "w");

            free(page_filename);
            free(full_path_file);

            char * page_recv = (char *) calloc(N_BYTES_TO_RECV, sizeof(char));
            // char * page_recv;
            // char page_recv[N_BYTES_TO_RECV];
            int bytes_recv = 0;
            // memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));

            while(1){
                  // page_recv = recvString(mysocket);
                  recvString2(page_recv, mysocket);
                  fputs(page_recv, p);
                  bytes_recv += strlen(page_recv);
                  //free(page_recv);
                  //printf(">>>>> %d\n", bytes_recv);
                  if( bytes_recv % n_bytes_to_recv == 0 ){
                      break;
                  }

                  memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));
            }
            free(page_recv);
            fclose(p);
            printf("\t=== Page received from proxy! ===\n");

            aux = aux->next;
        }
        free(aux);
        removeList(commandList);

    }else{
        printf("No arguments to search command!\n");
    }
}

// Lista os sites da proxy
void list( int mysocket ){

    puts("> List");

    int j = 0;
    while ( j < TABLE_SIZE ){

        int n = recvInt(mysocket);

        if( n != 0 ){
            char * site;
            for (int i = 1; i <= n; i++ ){
              site = recvString(mysocket);
              printf("[%d] (%d) %s\n", j, i, site);
              free(site);
            }
        }
        j++;
    }

}

void exit_( int * run, ShellCommands * history ){
    *run = FALSE;
    removeHistory(history);
    puts("Client says bye bye!");
}

int processCommand(char * command, int * run, ShellCommands * history, int mysocket, char * shared_directory ){

    char * strings = strtok(command, " ");

    int command_id;

    if( !strcmp( strings, SEARCH ) ){
        command_id = SEARCH_ID;
        sendInt(command_id, mysocket); // Envia o ID do comando a ser processado
        search(strings, shared_directory, mysocket);

    }else if( !strcmp( strings, LIST ) ){
        command_id = LIST_ID;
        sendInt(command_id, mysocket);
        list(mysocket);

    }else if( !strcmp( strings, EXIT ) ){
        command_id = EXIT_ID;
        sendInt(command_id, mysocket);
        exit_(&(*run), history);

    }else if( !strcmp( strings, HISTORY ) ){
        command_id = HISTORY_ID;
        sendInt(command_id, mysocket);
        printHistory(history);

    }else{
        command_id = COMMAND_ID_NOT_FOUND;
        sendInt(command_id, mysocket);
        printf( "Command not found!\n" );
    }

    return command_id;
}

void * readCommand( ShellCommands * history ){

    char * command = (char *) calloc(MAX_LINE_READ_COMMAND_SIZE, sizeof(char));

    printf(":~$ ");
    scanf(" %[^\n]%*c", command);

    insertCommand(command, history);

    return command;
}

int clientConnection( int port ){

    int mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest, 0, sizeof(dest));               /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);/* set destination IP number - localhost, 127.0.0.1*/
    dest.sin_port = htons( port );   /* set destination port number */

    int connectResult = connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

    if( connectResult == COMMUNICATION_ERROR ){
        printf("CLIENT ERROR: %s\n", strerror(errno));
        return COMMUNICATION_ERROR;
    }

    return mysocket;
}

int main( int argc, char *argv[] ){

    if( argc != 3 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    int port = atoi( argv[ 2 ] );

    char * shared_directory = ( char * ) calloc ( MAX_LINE_PATH_SHARED_FOLDER, sizeof(char) );
    strcpy( shared_directory, argv[ 1 ] );

    char * command;
    int run = TRUE;
    ShellCommands * history = createHistory();
    int mysocket = clientConnection(port);

    printf("[ PORTA ] %d\n[ SHARED DIRECTORY ] %s\n[ CLIENT SOCKET ] %d\n", port, shared_directory, mysocket);

    if(mysocket == COMMUNICATION_ERROR){
        free(history);
        return COMMUNICATION_ERROR;
    }

    while(run){

        command = readCommand(history);

        int command_id = processCommand(command, &run, history, mysocket, shared_directory);

        free(command);

    }

    close(mysocket);
    free(shared_directory);
    return EXIT_SUCCESS;
}
