#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#include "../include/communication.h"

int main(int argc, char *argv[])
{

    if( argc != 2 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    char* msg[5] = {"Msg1\n", "Msg2\n","Msg3\n","Msg4\n","Msg5\n"};

    srand( time( NULL ));

    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mysocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons( atoi( argv[ 1 ] ) );           /* set the server port number */

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    /* bind serv information to mysocket */
    bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(mysocket, 1);


    printf("Server is waiting for connections on port:%s\n", argv[ 1 ] );

    int consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);

    while(consocket)
    {
        printf("Incoming connection from %s - sending welcome\n", inet_ntoa(dest.sin_addr));

        //int randomMessage = rand() % 5;

        // Envio de string para o cliente
        // char * stringMsg = (char *) calloc(MAXRCVLEN, sizeof(char));
        // strcpy(stringMsg, "oi");
        // sendString(stringMsg, consocket);

        // Envio de int para o cliente
        // int intNumber = 5;
        // sendInt(intNumber, consocket);
        // printf(">> %d\n", intNumber);

        // Envio de double para o cliente
        // double doubleNumber = 5.5;
        // sendDouble(doubleNumber, consocket);
        // printf(">> %.1f\n", doubleNumber);

        // Recebimento de string do cliente
        // char * string = recvString(consocket);
        // printf(">> %s\n", string);

        // Recebimento de inteiro do cliente
     	  // int num = 0;
     	  // num = recvInt(consocket);
     	  // printf(">> %d\n", num);

     	  // Recebimento de double do cliente
     	  double doubleNumber = 0.0;
     	  doubleNumber = recvDouble(consocket);
     	  printf(">> %.1f\n", doubleNumber);

        close(consocket);
        consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
    }

    close(mysocket);
    return EXIT_SUCCESS;
}
