/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais				                        */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/argsList.h"

typedef struct List{
    int n_elements;
    struct Node *begin;
    struct Node *end;
}List;

typedef struct Node{
    char *argument;
    struct Node *next;
}Node;

List *listStructure = NULL;
Node *newnode = NULL;

void createList(){
    listStructure = (List *) calloc(1, sizeof(List));
    // listStructure->begin = NULL;
    // listStructure->end = NULL;
    // listStructure->n_elements = 0;
}

void insertArg(char * StringArgument){

    newnode = (Node *) calloc(1, sizeof(Node));
    newnode->argument = (char *) calloc(LIST_MAX_LINE_SIZE, sizeof(char));
    strcpy(newnode->argument, StringArgument);
    newnode->next = NULL;

    if(listStructure->begin == NULL){
        listStructure->begin = newnode;
    }else{
        (listStructure->end)->next = newnode;
    }
    listStructure->end = newnode;
    listStructure->n_elements += 1;
}

void printList(){
    Node *aux = listStructure->begin;
    int i = 1;

    while( aux->next != NULL ){
        printf("[%d] %s\n", i++, aux->argument);
        aux = aux->next;
    }

    printf("[%d] %s\n", i, aux->argument);
}

void removeList(){

    Node *aux = listStructure->begin;
    Node *tmp;

    while(aux->next != NULL){

        tmp = aux;
        aux = aux->next;
        listStructure->begin = aux;

        // tmp->next = NULL;
        // tmp->argument = NULL;
        free(tmp);
    }

    // listStructure->n_arguments = 0;
    // listStructure->begin = NULL;
    // listStructure->end = NULL;
    free(listStructure);

    // aux->argument = NULL;
    free(aux);
    return;

}
