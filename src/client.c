/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#include "../include/history.h"
#include "../include/argsList.h"
#include "../include/communication.h"

#define MAX_LINE_SIZE 1000
#define HASHTABLE_SIZE 10

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
void search(char *command, int mysocket){

    puts("> Search");

    List * commandList = determineArguments(command);

    int n = commandList->n_elements;
    // Envia o número de argumentos (sites)
    sendInt(n, mysocket);

    if(n){
        Node * aux = commandList->begin;

        for( int i = 0; i < n; i++ ){
            // Envia o argumento (site) para o Servidor
            sendString(aux->argument, mysocket);
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
    while ( j < HASHTABLE_SIZE ){

        int n = recvInt(mysocket);

        if( n != 0 ){
            char * site;
            for (int i = 1; i <= n; i++ ){
              site = recvString(mysocket);
              printf("[%d] (%d) %s\n", j, i, site);

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

int processCommand(char * command, int * run, ShellCommands * history, int mysocket){

    char * strings = strtok(command, " ");

    int command_id;

    if( !strcmp( strings, SEARCH ) ){
        command_id = SEARCH_ID;
        sendInt(command_id, mysocket); // Envia o ID do comando a ser processado
        search(strings, mysocket);

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

    char * command = (char *) calloc(MAX_LINE_SIZE, sizeof(char));

    printf(":~$ ");
    scanf(" %[^\n]%*c", command);
    
    insertCommand(command, history);

    return command;
}

int clientConnection(char *argv[]){

    int mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest, 0, sizeof(dest));               /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);/* set destination IP number - localhost, 127.0.0.1*/
    dest.sin_port = htons(  atoi( argv[ 1 ]) );   /* set destination port number */

    int connectResult = connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

    if( connectResult == -1 ){
        printf("CLIENT ERROR: %s\n", strerror(errno));
        return -1;
    }

    return mysocket;
}

int main( int argc, char *argv[] ){

    if( argc != 2 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    char * command;
    int run = TRUE;
    ShellCommands * history = createHistory();
    int mysocket = clientConnection(argv);

    if(mysocket == -1){
        return -1;
    }

    while(run){

        command = readCommand(history);

        int command_id = processCommand(command, &run, history, mysocket);

        free(command);

    }

    close(mysocket);
    return EXIT_SUCCESS;
}
