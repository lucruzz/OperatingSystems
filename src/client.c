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

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct ClientInformation{
    struct sockaddr_in * serv_dest;
    char * directory;
    char * str_aux;
    int clientSocket;
    int port;
}ClientInformation;

typedef struct SearchInformation_t{
    struct sockaddr_in * serv_dest;
    struct sockaddr_in t;
    char * directory;
    char * site;
    int clientSocket;
    int port;
    int tid;
}SearchInformation_t;


int clientConnection2( struct sockaddr_in serv_dest ){

    int mysocket = socket(AF_INET, SOCK_STREAM, 0);

    if( mysocket == COMMUNICATION_ERROR ){
        perror("[SOCKET CREATION ERROR]:");
        return COMMUNICATION_ERROR;
    }

    int connectResult = connect(mysocket, (struct sockaddr *)&serv_dest, sizeof(struct sockaddr_in));

    //printf("Incoming connections from %s\n", inet_ntoa(serv_dest.sin_addr));

    if( connectResult == COMMUNICATION_ERROR ){
        printf("CLIENT ERROR: %s\n", strerror(errno));
        return COMMUNICATION_ERROR;
    }

    return mysocket;
}

int clientConnection( int port, struct sockaddr_in serv_dest ){

    int mysocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_dest, 0, sizeof(serv_dest));               // zero the struct
    serv_dest.sin_family = AF_INET;
    serv_dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // set destination IP number - localhost, 127.0.0.1
    serv_dest.sin_port = htons( port );   // set destination port number

    int connectResult = connect(mysocket, (struct sockaddr *)&serv_dest, sizeof(struct sockaddr_in));

    //printf("Incoming connections from %s\n", inet_ntoa(serv_dest.sin_addr));

    if( connectResult == COMMUNICATION_ERROR ){
        printf("CLIENT ERROR: %s\n", strerror(errno));
        return COMMUNICATION_ERROR;
    }

    return mysocket;
}

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

    SearchInformation_t * pt = (SearchInformation_t *) ptr;

    int sendSocket = socket(AF_INET, SOCK_STREAM, 0);
    pt->clientSocket = sendSocket;
    int connectResult = connect(pt->clientSocket, (struct sockaddr *)&(pt->t), sizeof(struct sockaddr_in));

    // Envio o sinal do commando para a nova thread
    sendInt(SEARCH_ID, pt->clientSocket);

    // Envio o sinal do commando para a nova thread
    sendInt(1, pt->clientSocket);

    // Envio o site a ser procurado
    sendString(pt->site, pt->clientSocket);

    int content_length = recvInt(pt->clientSocket);
    printf("[+] Content-Length: %d\n", content_length);

    char * filename = recvString(pt->clientSocket);
    printf("[+] Filename: %s\n", filename);

    int total_full_length_to_file = strlen(pt->directory) + strlen(filename);
    // Aloco uma string com o tamanho total do caminho para o arquivo
    char * full_path_file = ( char * ) calloc(total_full_length_to_file + 2, sizeof(char));
    strcat(full_path_file, pt->directory);
    strcat(full_path_file, "/");
    strcat(full_path_file, filename);
    printf("[+] Path to file: %s\n", full_path_file);

    // Abro o arquivo para armazenar a página "no lado" cliente
    FILE * p = fopen(full_path_file, "w");

    free(filename);
    free(full_path_file);

    // Aloco memória para receber os bytes da proxy
    char * page_recv = (char *) calloc(N_BYTES_TO_RECV + 1, sizeof(char));

    int bytes_recv = 0;

    while(1){

        // int bytes = recvString2(page_recv, mysocket);
        int bytes = recv( pt->clientSocket, page_recv, N_BYTES_TO_RECV*sizeof(char), 0);
        // int bytes = recvString2(page_recv, pt->clientSocket);

        fputs(page_recv, p);

        bytes_recv += bytes;

        if( bytes_recv % content_length == 0){
            // printf("bytes reads> %d\n", bytes_recv);
            break;
        }

        memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));
    }

    free(page_recv);
    fclose(p);
    printf("\t=== Page received from proxy! ===\n");

    // Envio um sinal para fechar o socket
    sendInt(EXIT_ID, pt->clientSocket);
    // printf("%s\n", s);
    // sendString("exit", pt->clientSocket);
/*
    // Sinalizo a inicialização da thread
    char * s = recvString(pt->clientSocket);
    printf("%s\n", s);
    free(s);

    // Envio o site a ser buscado
    sendString(pt->site, pt->clientSocket);

    int found = recvInt(pt->clientSocket);

    if(found == NODE_NOT_FOUND){
        printf("[+] Proxy is searching on the internet!\n");
        // int content_length = recvInt(pt->clientSocket);
        // printf("[+] content-Length: %d\n", content_length);
    }else{
        printf("[+] Page on proxy!\n");
    }
  */
/*
    // Recebo a quantidade de bytes da página a ser recebida
    int n_bytes_to_recv = recvInt(pt->clientSocket);
    printf("\t=== Content-Length: %d ===\n", n_bytes_to_recv);

    // Recebo o nome da paǵina (http.c)
    char * page_filename = recvString(pt->clientSocket);

    // Eu acho que eu não preciso saber o diretório... Porque eu vou receber com o socket
    // Envio o nome do diretório (http.c)
    // sendString(pt->directory, pt->clientSocket);

    int length_str_page_filename = strlen(page_filename);
    int length_str_directory = strlen(pt->directory);

    int total_full_length_to_file = length_str_directory + length_str_page_filename;

    // Aloco uma string com o tamanho total do caminho para o arquivo
    char * full_path_file = ( char * ) calloc(total_full_length_to_file + 2, sizeof(char));
    strcat(full_path_file, pt->directory);
    strcat(full_path_file, "/");
    strcat(full_path_file, page_filename);

    // Abro o arquivo para armazenar a página "no lado" cliente
    FILE * p = fopen(full_path_file, "w");

    free(page_filename);
    free(full_path_file);

    // char * page_recv = (char *) calloc(N_BYTES_TO_RECV + 1, sizeof(char));
    char * page_recv = (char *) calloc(N_BYTES_TO_RECV, sizeof(char));

    int bytes_recv = 0;

    while(1){

        // int bytes = recvString2(page_recv, mysocket);
        // int bytes = recv( pt->clientSocket, page_recv, N_BYTES_TO_RECV*sizeof(char), 0);
        int bytes = recvString2(page_recv, pt->clientSocket);
        // printf("strlen(): %d | bytes reads: %d\n", (int)strlen(page_recv), bytes);

        fputs(page_recv, p);

        bytes_recv += bytes;
        //printf("bytes reads> %d\n", bytes_recv);

        // if(bytes % N_BYTES_TO_RECV != 0)
        //    break;

        if( bytes_recv % n_bytes_to_recv == 0){
            printf("bytes reads> %d\n", bytes_recv);
            break;
        }

        memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));
    }

    free(page_recv);
    fclose(p);
    printf("\t=== Page received from proxy! ===\n");
*/
    close(pt->clientSocket);

    return (void *) 0;
}

// Busca n sites
void search(char *command, ClientInformation * ptr){

    puts("> Search");

    int mysocket = ptr->clientSocket;
    char * directory = ptr->directory;
    struct sockaddr_in serv_dest = *(ptr->serv_dest);

    // Cria uma lista encadeada com os argumentos (sites/urls) do comando
    List * commandList = determineArguments(command);

    // Armazeno o número de argumentos processados no comando search
    int n = commandList->n_elements;

    // Envia o número de argumentos (sites)
    sendInt(n, mysocket);

    // Se o número de sites que o cliente pediu para processar for 1
    // Eu não quero/preciso abrir uma thread eu posso executar na própria thread
    if( n == 1 ){
        // Envio o site a ser procurado
        sendString((commandList->begin)->argument, ptr->clientSocket);

        int content_length = recvInt(ptr->clientSocket);
        printf("[+] Content-Length: %d\n", content_length);

        char * filename = recvString(ptr->clientSocket);
        printf("[+] Filename: %s\n", filename);

        int total_full_length_to_file = strlen(ptr->directory) + strlen(filename);

        // Aloco uma string com o tamanho total do caminho para o arquivo
        char * full_path_file = ( char * ) calloc(total_full_length_to_file + 2, sizeof(char));
        strcat(full_path_file, ptr->directory);
        strcat(full_path_file, "/");
        strcat(full_path_file, filename);
        printf("[+] Path to file: %s\n", full_path_file);

        // Abro o arquivo para armazenar a página "no lado" cliente
        FILE * p = fopen(full_path_file, "w");

        free(filename);
        free(full_path_file);

        // Aloco memória para receber os bytes da proxy
        char * page_recv = (char *) calloc(N_BYTES_TO_RECV + 1, sizeof(char));

        int bytes_recv = 0;

        while(1){

            // int bytes = recvString2(page_recv, mysocket);
            int bytes = recv( ptr->clientSocket, page_recv, N_BYTES_TO_RECV*sizeof(char), 0);
            // int bytes = recvString2(page_recv, pt->clientSocket);

            fputs(page_recv, p);

            bytes_recv += bytes;

            if( bytes_recv % content_length == 0){
                printf("bytes reads> %d\n", bytes_recv);
                break;
            }

            memset(page_recv, 0, N_BYTES_TO_RECV*sizeof(char));
        }

        free(page_recv);
        fclose(p);
        printf("\t=== Page received from proxy! ===\n");

    }else if(n > 1){
        // Cria uma estrutura para armazenar os threads_ids de acordo com o número de sites
        pthread_t search_t[ n ];

        // Envia o diretório compartilhado a que deseja receber os dados
        // sendString(directory, mysocket);

        // Armazeno em um ponteiro auxiliar o início da lista de argumentos
        //Node * aux = commandList->begin;
        Node * list_ptr[ n ];
      	createListOfPointers( list_ptr, commandList );
        SearchInformation_t searchArgs_t[ n ];

        memset(&searchArgs_t, 0, n*sizeof(SearchInformation_t));
        for ( int i = 0; i < n; i++ ){

            // int sendSocket = socket(AF_INET, SOCK_STREAM, 0);
            // searchArgs_t[ i ].clientSocket = sendSocket;
            searchArgs_t[ i ].directory = ptr->directory;
            searchArgs_t[ i ].site = list_ptr[ i ]->argument;
            searchArgs_t[ i ].serv_dest = (ptr->serv_dest);
            searchArgs_t[ i ].t = *(ptr->serv_dest);
            searchArgs_t[ i ].tid = i;

            int error_t = pthread_create( &search_t[ i ], NULL, &search_function_multithread, &searchArgs_t[ i ] );

            if(error_t){
                printf("\t=== Sorry! The thread could not be created! ===\n");
                continue;
            }
        }

        void * retVal = NULL;

        for( int j = 0; j < n; j++ ){

            int success = pthread_join( search_t[ j ], retVal );
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
    printf("[+] Sites availables on proxy:\n");
    while ( j < TABLE_SIZE ){

        int n = recvInt(mysocket);

        if( n != 0 ){
            char * site;
            char * time;
            for (int i = 1; i <= n; i++ ){
              site = recvString(mysocket);
              time = recvString(mysocket);
              printf("\tHashtable[%d](%d) %s | Creation time: %s\n", j, i, site, time);
              free(site);
              free(time);
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

    int mysocket = pt->clientSocket;

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


int main( int argc, char *argv[] ){

    if( argc != 3 ){
        printf("USAGE: server port_number\n");
        return EXIT_FAILURE;
    }

    int port = atoi( argv[ 2 ] );
    struct sockaddr_in serv_dest;
/////////////////////////////////// Esta parte está na função /////////////////////////////////////////

    memset(&serv_dest, 0, sizeof(serv_dest));               // zero the struct
    serv_dest.sin_family = AF_INET;
    serv_dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // set destination IP number - localhost, 127.0.0.1
    serv_dest.sin_port = htons( port );   // set destination port number

/////////////////////////////////// Esta parte está na função /////////////////////////////////////////

    int mysocket = clientConnection2(serv_dest);
    //int mysocket = clientConnection(port, serv_dest);

    if(mysocket == COMMUNICATION_ERROR){
        // free(history);
        return COMMUNICATION_ERROR;
    }

    char * directory = argv[ 1 ];//( char * ) calloc ( MAX_LINE_PATH_SHARED_FOLDER, sizeof(char) );
    //strcpy( directory, argv[ 1 ] ;
    char * command;
    int run = TRUE;
    ShellCommands * history = createHistory();

    ClientInformation * pt = (ClientInformation *) calloc( 1, sizeof(ClientInformation) );
    pt->port = port;
    pt->directory = directory;
    pt->clientSocket = mysocket;
    pt->serv_dest = &serv_dest;

    if ( pthread_mutex_init(&lock, NULL) != 0){
      printf("Mutex init failed\n");
      return EXIT_FAILURE;
    }

    printf("[+] You are connected to %s!\n", inet_ntoa((pt->serv_dest)->sin_addr));

    while(run){

        command = readCommand(history);

        int command_id = processCommand(command, &run, history, pt);

        free(command);

    }

    pthread_mutex_destroy(&lock);
    close(mysocket);
    //free(directory);
    free(pt);
    return EXIT_SUCCESS;
}
