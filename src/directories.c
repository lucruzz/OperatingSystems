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
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>

bool removeDirectory( char * directory_name ){

    int error = rmdir( directory_name );

  	if ( !error ){
    		printf("Directory %s removed!\n", directory_name);
        return true;
  	}

  	printf("Error: directory %s could not be removed!\n", directory_name);
  	return false;
}

bool removeFile( char * directory_name, char * filename ){

    DIR * p = opendir(directory_name);

    int error = unlinkat(dirfd(p), filename, 0);

    closedir( p );

    if( !error ){
        printf("File %s removed!\n", filename);
        return true;
    }

    printf("Error: File %s not removed\n", filename);
    return false;
}

bool makeDirectory( char * directory_name ){

    DIR * t = opendir(directory_name);

    if( t != NULL ) {
        close( dirfd(t) );
        removeDirectory( directory_name );
    }
    int error = mkdir( directory_name, 0777 );

    if ( !error ){
        printf("Directory %s created!\n", directory_name);
        return true;
    }

    printf("Error: directory not created!\n");
    return false;

}
