/*======================================================*/
/* Lucas Cruz                                           */
/* Engenharia de Computação - CEFET/RJ - UneD Petrópolis*/
/* Sistemas Operacionais                                */
/* Trabalho de implementação de um Servidor Proxy       */
/*======================================================*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "../include/history.h"
#include "../include/argsList.h"
#include "../include/directories.h"
#include "../include/communication.h"
#include "../include/hashServer.h" // TABLE_SIZE

#define MAX_LINE_READ_COMMAND_SIZE 1000
#define MAX_LINE_PATH_SHARED_FOLDER 20

sem_t sem1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct ClientInformation{
    int socket;
    int port;
    char * shared_directory;
    char * str_aux;
}ClientInformation;

typedef struct SearchInformation_t{
    int socket;
    char * shared_directory;
    char * site;
}SearchInformation_t;

List * determineArguments(char * commandArgs){

    int n_arguments = 0;
    List * commandList = createList();

    commandArgs = strtok(NULL, " ");

    while( commandArgs != NULL ){
        ++n_arguments;
        insertArg(commandArgs, commandList);
        commandArgs = strtok(NULL, " ");
    }

    return commandList;
}

Node * createListOfPointers( Node * lista [], List * command_list ){

    Node * pt = command_list->begin;
    int n = command_list->n_elements;

    for( int i = 0; i < n; i++ ){
        lista[ i ] = pt;
        pt = pt->next;
    }

    return * lista;
}

void * search_function_multithread( void * ptr ){

    //ClientInformation * pt = (ClientInformation *) ptr;
    //char * str_argument = pt->str_aux;
    // sem_post(&sem1);
    SearchInformation_t * pt = (SearchInformation_t *) ptr;
    char * str_argument = pt->site;
    char * shared_directory = pt->shared_directory;
    int mysocket = pt->socket;

    // Pego o tamanho do caminho do diretório
    int length_str_shared_directory = strlen( shared_directory );

    // Envia o argumento (site) para o Servidor
    sendString(str_argument, mysocket);

    //printf("> %d\n> %s\n> %s\n", mysocket, shared_directory, str_argument);

    //pthread_mutex_lock(&mutex);
    // Recebo a quantidade de bytes da página a ser recebida
    int n_bytes_to_recv = recvInt(mysocket);

    printf("\t=== Content-Length: %d ===\n", n_bytes_to_recv);

    // Recebo o nome da paǵina
    char * page_filename = recvString(mysocket);
    //pthread_mutex_unlock(&mutex);
    /*
    char * file_ptr = memrchr(str_argument, '/', (int)strlen(str_argument)) + 1;
    char * page_filename = (char *) calloc ((int)strlen(file_ptr) + 1, sizeof(char) );
    strcpy(page_filename, file_ptr);
    */
    printf("\t=== Filename: %s ===\n", page_filename);
    free(page_filename);

/*
    // Pego o tamanho do nome da página
    int length_str_page_filename = strlen(page_filename);

    int total_full_length_to_file = length_str_shared_directory + length_str_page_filename;

    // Aloco uma string para o tamanho total para o arquivo "no lado do" cliente
    char * full_path_file = ( char * ) calloc(total_full_length_to_file + 2, sizeof(char));
    strcat(full_path_file, shared_directory);
    strcat(full_path_file, "/");
    strcat(full_path_file, page_filename);

    // Abro o arquivo para armazenar a página "no lado" cliente
    FILE * p = fopen(full_path_file, "w");

    free(page_filename);
    free(full_path_file);

    char * page_recv = (char *) calloc(N_BYTES_TO_RECV + 1, sizeof(char));
    int bytes_recv = 0;

    while(1){

          int bytes = recvString2(page_recv, mysocket);

          fputs(page_recv, p);

          bytes_recv += bytes;

          if(bytes % N_BYTES_TO_RECV != 0)
              break;

          if( bytes_recv % n_bytes_to_recv == 0){
              break;
          }

          memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));
    }
    free(page_recv);
    fclose(p);
    printf("\t=== Page received from proxy! ===\n");
*/
    return (void *) 0;
}
/*
void search_function_single_thread(){
    // Armazeno em um ponteiro auxiliar o início da lista de argumentos
    Node * aux = commandList->begin;

    // Pego o tamanho do caminho do diretório
    int length_str_shared_directory = strlen(shared_directory);

    for( int i = 0; i < n; i++ ){
        // Envia o argumento (site) para o Servidor
        sendString(aux->argument, mysocket);

        // Recebo a quantidade de bytes da página a ser recebida
        int n_bytes_to_recv = recvInt(mysocket);
        printf("\t=== Content-Length: %d ===\n", n_bytes_to_recv);

        // Recebo o nome da paǵina
        char * page_filename = recvString(mysocket);
        // Pego o tamanho do nome da página
        int length_str_page_filename = strlen(page_filename);

        int total_full_length_to_file = length_str_shared_directory + length_str_page_filename;

        // Aloco uma string para o tamanho total para o arquivo "no lado do" cliente
        char * full_path_file = ( char * ) calloc(total_full_length_to_file + 2, sizeof(char));
        strcat(full_path_file, shared_directory);
        strcat(full_path_file, "/");
        strcat(full_path_file, page_filename);

        // Abro o arquivo para armazenar a página "no lado" cliente
        FILE * p = fopen(full_path_file, "w");

        free(page_filename);
        free(full_path_file);

        char * page_recv = (char *) calloc(N_BYTES_TO_RECV + 1, sizeof(char));

        int bytes_recv = 0;

        while(1){

              int bytes = recvString2(page_recv, mysocket);

              fputs(page_recv, p);


              bytes_recv += bytes;

              if(bytes % N_BYTES_TO_RECV != 0)
                  break;

              if( bytes_recv % n_bytes_to_recv == 0){
                  break;
              }

              memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));
        }
        free(page_recv);
        fclose(p);
        printf("\t=== Page received from proxy! ===\n");

        aux = aux->next;
    }
    free(aux);
    removeList(commandList);
}*/

// Busca n sites
void search(char *command, ClientInformation * ptr){

    puts("> Search");

    int mysocket = ptr->socket;
    char * shared_directory = ptr->shared_directory;

    // Cria uma lista encadeada com os argumentos (sites/urls) do comando
    List * commandList = determineArguments(command);

    // Armazeno o número de argumentos processados no comando search
    int n = commandList->n_elements;

    // Cria uma estrutura para armazenar os threads_ids de acordo com o número de sites
    pthread_t search_t[ n ];

    // Envia o número de argumentos (sites)
    sendInt(n, mysocket);

    // Envia o diretório compartilhado a que deseja receber os dados
    sendString(shared_directory, mysocket);

    if(n != 0){
        // Armazeno em um ponteiro auxiliar o início da lista de argumentos
        //Node * aux = commandList->begin;
        Node * list_ptr[ n ];
      	createListOfPointers( list_ptr, commandList );
        SearchInformation_t search_t_id[ n ];

        for ( int i = 0; i < n; i++ ){
          search_t_id[ i ].socket = mysocket;
          search_t_id[ i ].shared_directory = shared_directory;
          search_t_id[ i ].site = list_ptr[ i ]->argument;
        }

        for ( int i = 0; i < n; i++ ){

            // armazena a string (site) na estrutura ClientInformation_t
            //ptr->str_aux = list_ptr[ i ]->argument;//aux->argument;

            // search_t_id[ i ].socket = mysocket;
            // search_t_id[ i ].shared_directory = shared_directory;
            // search_t_id[ i ].site = list_ptr[ i ]->argument;
            //if( i == 1)
            //    printf("%d %s %s\n", search_t_id[ i ].socket, search_t_id[ i ].shared_directory, search_t_id[ i ].site );
            // printf("%d %s %s\n", ptr->socket, ptr->shared_directory, ptr->str_aux );

            // cria uma thread para processar a busca
            int error_t = pthread_create( &search_t[ i ], NULL, search_function_multithread, &search_t_id[ i ] );
            //int error_t = pthread_create( &search_t[ i ], NULL, search_function_multithread, ptr );

            if(error_t){
                printf("\t=== [search] Error: Thread could not be created! ===\n");
            }

        }

        void * returnValue = NULL;

        for( int j = 0; j < n; j++ ){

            int success = pthread_join( search_t[ j ], returnValue );
            if(success != 0){
                printf("Error on thread_id %ld!\n", search_t[ j ]);
            }
        }

        //free(aux);
        removeList(commandList);

        // Executes search_function_single_thread();

    }else{
        printf("\t==== No arguments to search command! ===\n");
    }
}

// Lista os sites da proxy
void list( int mysocket ){

    puts("> List");

    int j = 0;
    while ( j < TABLE_SIZE ){

        int n = recvInt(mysocket);

        if( n != 0 ){
            char * site;
            for (int i = 1; i <= n; i++ ){
              site = recvString(mysocket);
              printf("[%d] (%d) %s\n", j, i, site);
              free(site);
            }
        }
        j++;
    }

}

void exit_( int * run, ShellCommands * history ){
    *run = FALSE;
    removeHistory(history);
    puts("Client says bye bye!");
}

int processCommand(char * command, int * run, ShellCommands * history, ClientInformation * pt ){

    int mysocket = pt->socket;

    char * strings = strtok(command, " ");

    int command_id;

    if( !strcmp( strings, SEARCH ) ){
        command_id = SEARCH_ID;
        sendInt(command_id, mysocket); // Envia o ID do comando a ser processado
        search(strings, pt);

    }else if( !strcmp( strings, LIST ) ){
        command_id = LIST_ID;
        sendInt(command_id, mysocket);
        list(mysocket);

    }else if( !strcmp( strings, EXIT ) ){
        command_id = EXIT_ID;
        sendInt(command_id, mysocket);
        exit_(&(*run), history);

    }else if( !strcmp( strings, HISTORY ) ){
        command_id = HISTORY_ID;
        sendInt(command_id, mysocket);
        printHistory(history);

    }else{
        command_id = COMMAND_ID_NOT_FOUND;
        sendInt(command_id, mysocket);
        printf( "Command not found!\n" );
    }

    return command_id;
}

void * readCommand( ShellCommands * history ){

    char * command = (char *) calloc(MAX_LINE_READ_COMMAND_SIZE, sizeof(char));

    printf(":~$ ");
    scanf(" %[^\n]%*c", command);

    insertCommand(command, history);

    return command;
}

int clientConnection( int port ){

    int mysocket;
    struct sockaddr_in dest;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest, 0, sizeof(dest));               // zero the struct
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // set destination IP number - localhost, 127.0.0.1
    dest.sin_port = htons( port );   // set destination port number

    int connectResult = connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

    if( connectResult == COMMUNICATION_ERROR ){
        printf("CLIENT ERROR: %s\n", strerror(errno));
        return COMMUNICATION_ERROR;
    }

    return mysocket;
}

int main( int argc, char *argv[] ){

    if( argc != 3 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    int port = atoi( argv[ 2 ] );

    char * shared_directory = ( char * ) calloc ( MAX_LINE_PATH_SHARED_FOLDER, sizeof(char) );
    strcpy( shared_directory, argv[ 1 ] );

    char * command;
    int run = TRUE;
    ShellCommands * history = createHistory();
    int mysocket = clientConnection(port);

    if(mysocket == COMMUNICATION_ERROR){
        free(history);
        return COMMUNICATION_ERROR;
    }

    //printf("[ PORTA ] %d\n[ SHARED DIRECTORY ] %s\n[ CLIENT SOCKET ] %d\n", port, shared_directory, mysocket);
    ClientInformation * pt = (ClientInformation *) calloc( 1, sizeof(ClientInformation) );
    pt->port = port;
    pt->shared_directory = shared_directory;
    pt->socket = mysocket;
    sem_init(&sem1, 0, 1);
    pthread_mutex_init(&mutex, NULL);

    while(run){

        command = readCommand(history);

        int command_id = processCommand(command, &run, history, pt);

        free(command);

    }

    //sem_destroy(&sem1);
    pthread_mutex_destroy(&mutex);
    close(mysocket);
    free(shared_directory);
    free(pt);
    return EXIT_SUCCESS;
}
