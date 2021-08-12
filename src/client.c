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

#define MAX_LINE_SIZE 500

void search(){
    puts("> Search");
}

void list(){
    puts("> List");
}

void exit_(int * run){
    *run = FALSE;
    removeHistory();
    puts("Client says bye bye!");
}

int determineArguments(char * commandArgs){

    int n_arguments = 0;
    createList();

    commandArgs = strtok(NULL, " ");

    while( commandArgs != NULL ){

        ++n_arguments;
        insertArg(commandArgs);
        commandArgs = strtok(NULL, " ");

    }

    return n_arguments;
}

int processCommand(char * command, int * run, int mysocket){

    char * strings = strtok(command, " ");

    int command_id;

    if( !strcmp( strings, SEARCH ) ){
        command_id = SEARCH_ID;
        search();

    }else if( !strcmp( strings, LIST ) ){
        command_id = LIST_ID;
        list();

    }else if( !strcmp( strings, EXIT ) ){
        command_id = EXIT_ID;
        exit_(&(*run));

    }else if( !strcmp( strings, HISTORY ) ){
        command_id = HISTORY_ID;
        printHistory();

    }else{
        command_id = COMMAND_ID_NOT_FOUND;
        printf( "Command not found!\n" );
    }

    sendInt(command_id, mysocket);

    return command_id;
}

void * readCommand(){

    char * command = (char *) calloc(MAX_LINE_SIZE, sizeof(char));

    printf(":~$ ");
    scanf(" %[^\n]%*c", command);

    insertCommand(command);

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
        return EXIT_FAILURE;
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
    int mysocket = clientConnection(argv);

    while(run){

        command = readCommand();

        int command_id = processCommand(command, &run, mysocket);

        if( command_id != EXIT_ID && command_id != COMMAND_ID_NOT_FOUND){

            int n_arguments = determineArguments(command);

            if (n_arguments){
                printList();
                removeList();
            }
        }

    }

    close(mysocket);
    return EXIT_SUCCESS;
}
