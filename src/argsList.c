/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/argsList.h"

List * createList(){
    List * listStructure = (List *) calloc(1, sizeof(List));
    return listStructure;
}

void insertArg(char * StringArgument, List * listStructure){

    Node * newnode = (Node *) calloc(1, sizeof(Node));
    // newnode->argument = (char *) calloc(LIST_MAX_LINE_SIZE, sizeof(char));
    // strcpy(newnode->argument, StringArgument);
    newnode->argument = StringArgument;
    newnode->next = NULL;

    if(listStructure->begin == NULL){
        listStructure->begin = newnode;
    }else{
        (listStructure->end)->next = newnode;
    }
    listStructure->end = newnode;
    listStructure->n_elements += 1;
}

void printList(List * listStructure){
    Node *aux = listStructure->begin;
    int i = 1;

    while( aux->next != NULL ){
        printf("[%d] %s\n", i++, aux->argument);
        aux = aux->next;
    }

    printf("[%d] %s\n", i, aux->argument);
    free(aux);
}

void removeList(List * listStructure){

    Node *aux = listStructure->begin;
    Node *tmp;

    while(aux->next != NULL){
        tmp = aux;
        aux = aux->next;
        listStructure->begin = aux;
        free(tmp);
    }

    free(listStructure);

    return;
}
