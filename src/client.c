/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais				                        */
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

#define SEARCH "search"
#define LIST "list"
#define EXIT "exit"
#define HISTORY "history"

#define SEARCH_ID 1
#define LIST_ID 2
#define EXIT_ID 3
#define HISTORY_ID 4
#define COMMAND_ID_NOT_FOUND -1

#define TRUE 1
#define FALSE 0

void search(){
    puts("> Search");
}

void list(){
    puts("> List");
}

void exit_(int * run){
    *run = FALSE;
    removeHistory();
    puts("Bye bye!");
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

int processCommand(char * command, int * run){

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

    return command_id;
}

void * readCommand(){

    char * command = (char *) calloc(MAX_LINE_SIZE, sizeof(char));

    printf(":~$ ");
    scanf(" %[^\n]%*c", command);

    insertCommand(command);

    return command;
}


int clientConnection(){

    if( argc != 2 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
    bzero( buffer, MAXRCVLEN + 1 );
    int len, mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest, 0, sizeof(dest));                /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
    dest.sin_port = htons(  atoi( argv[ 1 ]) );                /* set destination port number */

    int connectResult = connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

    if( connectResult == -1 ){

        printf("CLIENT ERROR: %s\n", strerror(errno));

        return EXIT_FAILURE;
    }

    close(mysocket);
    return EXIT_SUCCESS;
}

int main( int argc, char **argv ){

    char * command;
    int run = TRUE;

    while(run){

        command = readCommand();

        int command_id = processCommand(command, &run);

        if( command_id != EXIT_ID && command_id != COMMAND_ID_NOT_FOUND){

            int n_arguments = determineArguments(command);

            if (n_arguments){
                printList();
                removeList();
            }
        }
    }
    return 0;
}
