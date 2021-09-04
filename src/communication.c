/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include "../include/communication.h"

void sendInt( int intNumber, int socket ){
    send(socket, &intNumber, sizeof(int), 0);
}

int recvInt( int socket ){
    int intNumber = 0;
    recv(socket, &intNumber, sizeof(int), 0);
    return intNumber;
}

void sendDouble( double doubleNumber, int socket ){
    send(socket, &doubleNumber, sizeof(double), 0);
}

double recvDouble( int socket ){
    double doubleNumber = 0.0;
    recv(socket, &doubleNumber, sizeof(double), 0);
    return doubleNumber;
}

void sendString( char * string, int socket ){
    int len_str =  strlen(string) + 1; // +1 para o '\0'
    sendInt(len_str, socket);
    send(socket, string, sizeof(char)*len_str, 0);
}

char * recvString( int socket ){
    int len_str = 0;
    len_str = recvInt(socket);
    char * string = (char *) calloc(len_str, sizeof(char)*len_str);
    recv(socket, string, len_str, 0);
    return string;
}
