/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#ifndef __DIRECTORIES_H__
#define __DIRECTORIES_H__

#define INFO_PAGES_DIRECTORY "infoPage"
#define PROXY_DIRECTORY "proxy"
#define LENGTH_DIR_PATH 50

bool removeDirectory( char * );
bool removeFile( char *, char * );
bool makeDirectory( char * );

#endif
