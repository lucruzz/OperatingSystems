#ifndef __HASHSERVER_H__
#define __HASHSERVER_H__

#define MAX_SITE_STR_LENGHT 500
#define TABLE_SIZE 10

typedef struct linkedList{
    char * site;
    struct linkedList * next;
}LinkedList;

typedef struct hash{
    int n_elements; // Número de elementos da lista encadeada
    LinkedList * begin; // Ponteiro para head da LinkedList
}Hash;

int hashFunction( char * );
LinkedList * createNode( char * , Hash [] );


#endif
