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
    //printf("intNumber (send): %d\n", intNumber);
    send(socket, &intNumber, sizeof(int), 0);
}

int recvInt( int socket ){
    int intNumber = 0;
    recv(socket, &intNumber, sizeof(int), 0);
    //printf("intNumber (recv): %d\n", intNumber);
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
    int len_str =  (int)strlen(string);
    sendInt(len_str, socket);
    send(socket, string, sizeof(char)*(len_str + 1), 0); // +1 para o '\0'
}

void sendString2( char * string, int socket ){
    int len_str = strlen(string);
    sendInt(len_str, socket);
    send(socket, string, sizeof(char)*(len_str + 1), 0);
    printf(">>>>>> ::::: %s\n", string);
}

char * recvString( int socket ){
    int len_str = recvInt(socket);
    //char * string = (char *) calloc(len_str, sizeof(char)*len_str);
    char * string = (char *) calloc(len_str + 1, sizeof(char));
    //char string[len_str];
    //memset(string, 0, len_str);
    recv(socket, string, sizeof(char)*(len_str + 1), 0);
    return string;
}

int recvString2( char * string, int socket ){
    int len_str = 0;
    len_str = recvInt(socket);
    int bytes = recv(socket, string, sizeof(char)*(len_str + 1), 0);
    return bytes;
}
