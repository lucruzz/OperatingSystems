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
#define LENGTH_STR_INFO_HTML 1000
#define HTTP_REQUEST_SIZE 1000
#define HTML_INFO_STR_SIZE 1000
#define ERROR_URL_NOT_HTTP -3

#define HTTP_STR "http://"

char * getWebsiteServer( char * );
struct addrinfo * getWebsiteSocket( char * );
int * conncetionWebsiteSocket( char * , struct addrinfo * );
char * getHTMLinformation( char * , int * );
int getHTMLlength( char * );
void getHTML( char *, int , int, int );
int getHTML_With_No_Length_Found( char *, int );
//int http( char * );
int http( char *, int * );
bool checkURL( char * );

#endif
