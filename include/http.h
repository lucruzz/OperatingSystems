/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de uma Proxy		            */
/*======================================================*/
#ifndef __HTTP_H__
#define __HTTP_H__

#define ERROR -1
#define CONTENT_LENGTH_NOT_FOUND -2
#define LENGTH_INFO_HTML 700
#define N_BYTES_TO_RECEIVE 10

char * treatingURL( char * );
struct addrinfo * getWebsiteSocket( char * );
int conncetionWebsiteSocket( char * , struct addrinfo * );
char * getHTMLinformation( char * , int );
int getHTMLlength( char * );
void getHTML( char *, int , int );
int getHTML_With_No_Length_Found( char *, int );
void http( char * );

#endif
