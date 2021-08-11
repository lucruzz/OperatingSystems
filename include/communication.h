#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#define MAXRCVLEN 500

void sendInt( int intNumber, int socket );
int recvInt( int socket );

void sendDouble( double doubleNumber, int socket );
double recvDouble( int socket );

void sendString( char* string, int socket );
char * recvString( int socket );

#endif
