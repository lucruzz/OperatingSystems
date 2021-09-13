/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#define MAXRCVLEN 500
#define N_BYTES_TO_RECV 500
#define N_BYTES_TO_SEND 500

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
#define COMMAND_ID_NOT_FOUND -2

#define COMMUNICATION_ERROR -1

void sendInt( int intNumber, int socket );
int recvInt( int socket );

void sendDouble( double doubleNumber, int socket );
double recvDouble( int socket );

void sendString( char* string, int socket );
char * recvString( int socket );

// Essa função recvString2 ela não aloca memória
// Útil para usar no recebimento das mensagens pelo cliente
void recvString2( char *, int );

#endif
