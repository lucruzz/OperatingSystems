#ifndef __HASHSERVER_H__
#define __HASHSERVER_H__

#define MAX_SITE_STR_LENGHT 500
#define TABLE_SIZE 10

typedef struct LinkedList{
    char * site;
    struct LinkedList * next;
}LinkedList;

typedef struct Hash{
    int n_elements; // Número de elementos da lista encadeada
    LinkedList * begin; // Ponteiro para head da LinkedList
}Hash;

int hashFunction( char * );
LinkedList * createNode( char * , Hash [] );
void removeHash( Hash [] );
void printHash( Hash [] );
LinkedList * searchInHash( char * , Hash [] );

#endif
