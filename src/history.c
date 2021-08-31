/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/history.h"

ShellCommands * createHistory(){
    return (ShellCommands *) calloc(1, sizeof(ShellCommands));
}

void insertCommand( char * command, ShellCommands * history ){

    ListCommand * newnode = (ListCommand *) calloc(1, sizeof(ListCommand));
    //newnode->command = command;
    int n = strlen(command) + 1;
    newnode->command = (char *) calloc ( n, sizeof(char) );
    strcpy(newnode->command, command);

    if(history->begin == NULL){
        history->begin = newnode;

    }else{
        ListCommand * aux = history->end;
        aux->next = newnode;
    }

    history->end = newnode; // adiciono o novo nó ao final da lista

    history->number_of_commands++;

    return;
}

void printHistory( ShellCommands * history ){

    ListCommand * aux = history->begin;
    //int number_of_command = history->number_of_commands;
    int number_of_command = 1;
    while( aux != NULL ){
        printf(" %d\t\b\b\b%s\n", number_of_command++, aux->command);
        aux = aux->next;
    }

    return;
}

void removeHistory( ShellCommands * history ){

    int n = history->number_of_commands;
    ListCommand * aux = history->begin;
    ListCommand * tmp;

    for (int i = 0; i < n; i++){

        tmp = aux->next;
        free(aux->command);
        free(aux);
        aux = tmp;
    }

    free(tmp);
    free(history);

    return;
}
