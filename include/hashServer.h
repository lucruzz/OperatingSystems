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
    char * site;
    int content_length;
    time_t creation_time;
    struct LinkedList * next;
}LinkedList;

typedef struct Hash{
    int n_elements; // Número de elementos da lista encadeada
    LinkedList * begin; // Ponteiro para head da LinkedList
}Hash;

int hashFunction( char * );
LinkedList * createNode( char * , int, Hash [] );
void removeHash( Hash [] );
LinkedList * remove_Hash_Node ( LinkedList *, LinkedList*, int, Hash [] );
void printHash( Hash [] );
LinkedList * searchInHash( char * , Hash [] );

#endif
