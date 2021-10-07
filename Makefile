CC=gcc
CFLAGS=-c -g -pthread -D_GNU_SOURCE
SOURCE_DIR=src
OBJ_DIR=obj
HTTP_SCRIPT=$(SOURCE_DIR)/http2.c
HTTP_OBJ=$(OBJ_DIR)/http2.o

all: bin/client bin/server

bin/client: obj/client.o obj/history.o obj/argsList.o obj/communication.o obj/directories.o
	gcc -o bin/client obj/client.o obj/history.o obj/argsList.o obj/communication.o obj/directories.o -pthread

obj/client.o: src/client.c src/history.c src/argsList.c src/communication.c
	$(CC) $(CFLAGS) src/client.c src/history.c src/argsList.c src/communication.c src/directories.c
	mv *.o obj/

bin/server: obj/server.o obj/hashServer.o obj/communication.o $(HTTP_OBJ) obj/directories.o
	gcc -o bin/server obj/server.o obj/hashServer.o obj/communication.o $(HTTP_OBJ) obj/directories.o -pthread

obj/server.o: src/server.c src/hashServer.c src/communication.c $(HTTP_SCRIPT) src/directories.c
	$(CC) $(CFLAGS) src/server.c src/hashServer.c src/communication.c $(HTTP_SCRIPT) src/directories.c
	mv *.o obj/

clean:
	clear
	rm clients/client1/* clients/client2/* clients/client3/* proxy/* infoPage/* obj/* bin/*
