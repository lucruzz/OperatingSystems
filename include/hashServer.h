/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#ifndef __HASHSERVER_H__
#define __HASHSERVER_H__

#define MAX_SITE_STR_LENGHT 700
#define TABLE_SIZE 10

typedef struct LinkedList{
    struct LinkedList * next;
    char * site;
    int content_length;
    time_t creation_time;
}LinkedList;

typedef struct Hash{
  LinkedList * begin; // Ponteiro para head da LinkedList
    int n_elements; // Número de elementos da lista encadeada
}Hash;
/*
int hashFunction( char * );
LinkedList * createNode( char * , int, Hash [] );
void removeHash( Hash [] );
LinkedList * remove_Hash_Node ( LinkedList *, LinkedList*, int, Hash [] );
void printHash( Hash [] );
LinkedList * searchInHash( char * , Hash [] );
*/

int hashFunction( char * );
LinkedList * createNode( char * , int,  Hash ** );
void removeHash( Hash ** );
void printHash( Hash ** );
LinkedList * searchInHash( char * , Hash ** );
LinkedList * remove_Hash_Node ( LinkedList * , LinkedList * , int, Hash ** );

#endif
