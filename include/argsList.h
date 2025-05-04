/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de uma Proxy               */
/*======================================================*/
#ifndef __ARGSLIST_H__
#define __ARGSLIST_H__

#define LIST_MAX_LINE_SIZE 200

typedef struct List{
    int n_elements;
    struct Node * begin;
    struct Node * end;
}List;

typedef struct Node{
    char * argument;
    struct Node * next;
}Node;

List * createList();
void insertArg( char * , List * );
void printList( List * );
void removeList( List * );

#endif
