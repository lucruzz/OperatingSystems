/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de uma Proxy		            */
/*======================================================*/
#ifndef __HTTP_H__
#define __HTTP_H__

#define ERROR -1

char * treatingURL( char * );
struct addrinfo * getWebsiteSocket( char * );
int conncetionWebsiteSocket( char * , struct addrinfo * );
void getHTMLinformation( int );
int getHTMLlength();
void getHTML( int , int );
void http( char * );

#endif
