#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/hashServer.h"

int hashFunction( char * site ){

    int sum = 0; // variavel para somar as letras (ASCII - decimal) e o número de letras
    int ascii_decimal; // recebe o número da letra referente a seu decimal na ASCII
    int n = strlen(site);

    for( int i = 0; i < n; i++ ){
        ascii_decimal = (int)(*site + i);
        sum = ascii_decimal + i + 1;
    }

    return (sum + n) % 10;
}

LinkedList * createNode( char * site, Hash hashArray[] ){

    LinkedList * newnode = (LinkedList *) calloc(1, sizeof(LinkedList));

    newnode->site = site;

    int index = hashFunction(site);

    if( hashArray[index].begin != NULL ){
        newnode->next = hashArray[index].begin;
    }
    hashArray[index].begin = newnode;
    hashArray[index].n_elements += 1;

    return newnode;
}
