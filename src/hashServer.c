/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

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

LinkedList * createNode( char * site, int content_length, Hash hashArray[] ){

    LinkedList * newnode = (LinkedList *) calloc(1, sizeof(LinkedList));
    newnode->site = site;
    newnode->content_length = content_length;
    newnode->creation_time = time( NULL );

    int index = hashFunction(site);

    if( hashArray[index].begin != NULL ){
        newnode->next = hashArray[index].begin;
    }
    hashArray[index].begin = newnode;
    hashArray[index].n_elements += 1;

    return newnode;
}

void printHash( Hash hashArray[] ){

    int i = 0;

    while( i < TABLE_SIZE ){

        if( hashArray[i].begin != NULL ){

            LinkedList * node = hashArray[i].begin;
            int n = hashArray[i].n_elements;

            for ( int j = 1; j <= n; j++ ){
                printf("[%d] (%d) %s\n", i, j, node->site);
                node = node->next;
            }

        }
        i++;

    }
}

LinkedList * searchInHash( char * site , Hash hashArray[] ){

    int indexHash = hashFunction(site);

    LinkedList * node = hashArray[indexHash].begin;

    while( node != NULL ){

        if( !strcmp(node->site, site) ){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

void removeHash( Hash hashArray[] ){

    int i = 0;
    while( i < TABLE_SIZE ){

        if( hashArray[i].begin != NULL ){

            LinkedList * aux = hashArray[i].begin;
            LinkedList * tmp;

            while( aux != NULL ){
                tmp = aux;
                hashArray[i].begin = aux->next;
                aux = hashArray[i].begin;
                free(tmp->site);
                free(tmp);
            }
        }
        i++;
    }
}


LinkedList * remove_Hash_Node ( LinkedList * previous_node, LinkedList * node, int index, Hash hashArray[] ){

    LinkedList * aux;
    if( previous_node == node ){
        hashArray[index].begin = node->next;
        aux = hashArray[index].begin;
    }else{
        previous_node->next = node->next;
        aux = previous_node->next;
    }

    hashArray[index].n_elements--;
    free(node->site);
    free(node);

    return aux;
}
