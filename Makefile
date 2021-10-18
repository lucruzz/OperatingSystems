CC=gcc
CC_FLAGS_OBJ=gcc -o
CFLAGS=-c -g -pthread -D_GNU_SOURCE

SOURCE_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

HTTP_SCRIPT=$(SOURCE_DIR)/http2.c
HTTP_OBJ=$(OBJ_DIR)/http2.o

SERVER_SCRIPT=$(SOURCE_DIR)/server.c
SERVER_OBJ=$(OBJ_DIR)/server.o
SERVER_BIN=$(BIN_DIR)/server

CLIENT_SCRIPT=$(SOURCE_DIR)/client.c
CLIENT_OBJ=$(OBJ_DIR)/client.o
CLIENT_BIN=$(BIN_DIR)/client

HISTORY_SCRIPT=$(SOURCE_DIR)/history.c
HISTORY_OBJ=$(OBJ_DIR)/history.o

LIST_SCRIPT=$(SOURCE_DIR)/argsList.c
LIST_OBJ=$(OBJ_DIR)/argsList.o

COMM_SCRIPT=$(SOURCE_DIR)/communication.c
COMM_OBJ=$(OBJ_DIR)/communication.o

HASH_SCRIPT=$(SOURCE_DIR)/hashServer2.c
HASH_OBJ=$(OBJ_DIR)/hashServer2.o

DIREC_SCRIPT=$(SOURCE_DIR)/directories.c
DIREC_OBJ=$(OBJ_DIR)/directories.o


all: $(CLIENT_BIN) $(SERVER_BIN)

bin/client: $(CLIENT_OBJ) $(HISTORY_OBJ) $(LIST_OBJ) $(COMM_OBJ) $(DIREC_OBJ)
	$(CC_FLAGS_OBJ) $(CLIENT_BIN) $(CLIENT_OBJ) $(HISTORY_OBJ) $(LIST_OBJ) $(COMM_OBJ) $(DIREC_OBJ) -pthread

obj/client.o: $(CLIENT_SCRIPT) $(HISTORY_SCRIPT) $(LIST_SCRIPT) $(COMM_SCRIPT)
	$(CC) $(CFLAGS) $(CLIENT_SCRIPT) $(HISTORY_SCRIPT) $(LIST_SCRIPT) $(COMM_SCRIPT) $(DIREC_SCRIPT)
	mv *.o $(OBJ_DIR)/

bin/server: $(SERVER_OBJ) $(HASH_OBJ) $(COMM_OBJ) $(HTTP_OBJ) $(DIREC_OBJ)
	$(CC_FLAGS_OBJ) $(SERVER_BIN) $(SERVER_OBJ) $(HASH_OBJ) $(COMM_OBJ) $(HTTP_OBJ) $(DIREC_OBJ) -pthread

obj/server.o: $(SERVER_SCRIPT) $(HASH_SCRIPT) $(COMM_SCRIPT) $(HTTP_SCRIPT) $(DIREC_SCRIPT)
	$(CC) $(CFLAGS) $(SERVER_SCRIPT) $(HASH_SCRIPT) $(COMM_SCRIPT) $(HTTP_SCRIPT) $(DIREC_SCRIPT)
	mv *.o $(OBJ_DIR)/

clean:
	rm clients/client1/* clients/client2/* clients/client3/* proxy/* infoPage/* $(OBJ_DIR)/* $(BIN_DIR)/*
	clear
