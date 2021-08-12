/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de uma Proxy		            */
/*======================================================*/
#ifndef __HISTORY_H__
#define __HISTORY_H__

#define HISTORY_MAX_LINE_LENGHT 500

typedef struct ShellCommands ShellCommands;
void insertCommand(char * command);
void removeHistory();
void printHistory();

#endif
