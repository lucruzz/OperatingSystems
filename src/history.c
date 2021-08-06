/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais				                        */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/history.h"

typedef struct ListCommand{
    char * command;
    struct ListCommand *next;
}ListCommand;

ListCommand *head = NULL, *newnode, *tail=NULL;
int command_counter = 0;

void insertCommand(char * command){

    newnode = (ListCommand *) calloc(1, sizeof(ListCommand));
    newnode->command = (char *) calloc(HISTORY_MAX_LINE_LENGHT, sizeof(char));
    strcpy(newnode->command, command);
    newnode->next = NULL;

    if(head == NULL){
        head = newnode;
    }else{
        tail->next = newnode;
    }

    tail = newnode;
    command_counter++;

    return;
}

void printHistory(){

    ListCommand * aux = head;
    int cont_comandos = 0;

    while( aux->next != NULL ){
        printf(" %d\t\b\b\b%s\n", ++cont_comandos, aux->command);
        aux = aux->next;
    }

    printf(" %d\t\b\b\b%s\n", ++cont_comandos, aux->command);

    return;
}

void removeHistory(){

    ListCommand *aux = head;

    while( aux->next != NULL ){

        head = aux->next;

        aux->command = NULL;
        aux->next = NULL;
        free(aux);

        aux = head;
    }

    aux->command = NULL;
    aux->next = NULL;

    free(aux);

    return;
}
