/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de uma Proxy		            */
/*======================================================*/
#ifndef __HISTORY_H__
#define __HISTORY_H__

#define HISTORY_MAX_LINE_LENGHT 500

typedef struct ShellCommands{
    int number_of_commands;
    struct ListCommand * begin;
    struct ListCommand * end;
}ShellCommands;

typedef struct ListCommand{
    char * command;
    struct ListCommand * next;
}ListCommand;

ShellCommands * createHistory();
void insertCommand( char * , ShellCommands * );
void printHistory( ShellCommands * );
void removeHistory( ShellCommands * );

#endif
