/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#define MAXRCVLEN 500

#define TRUE 1
#define FALSE 0

#define SEARCH "search"
#define LIST "list"
#define EXIT "exit"
#define HISTORY "history"

#define SEARCH_ID 1
#define LIST_ID 2
#define EXIT_ID 3
#define HISTORY_ID 4
#define COMMAND_ID_NOT_FOUND -1

void sendInt( int intNumber, int socket );
int recvInt( int socket );

void sendDouble( double doubleNumber, int socket );
double recvDouble( int socket );

void sendString( char* string, int socket );
char * recvString( int socket );

#endif
