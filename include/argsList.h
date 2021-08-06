/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais				                        */
/* Trabalho de implementação de uma Proxy		            */
/*======================================================*/
#ifndef __LIST_H__
#define __LIST_H__

#define LIST_MAX_LINE_SIZE 200

typedef struct List List;
typedef struct Node Node;

void createList();
void insertArg(char * StringArgument);
void printList();
void removeList();

#endif
