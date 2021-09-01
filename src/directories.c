/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

bool removeDirectory( char * directory_name ){

    int error = rmdir( directory_name );

  	if ( !error ){
    		printf("Directory %s created!\n", directory_name);
        return true;
  	}

  	printf("Error: directory %s could not be removed!\n", directory_name);
  	return false;
}

bool makeDirectory( char * directory_name ){

    int error = mkdir( directory_name, 0777 );

  	if ( !error ){
    		printf("Directory %s created!\n", directory_name);
        return true;
  	}

    printf("Error: directory %s could not be created!\n", directory_name);
  	return false;

}
