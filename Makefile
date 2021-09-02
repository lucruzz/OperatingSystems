all: bin/client bin/server

bin/client: obj/client.o obj/history.o obj/argsList.o obj/communication.o
	gcc -o bin/client obj/client.o obj/history.o obj/argsList.o obj/communication.o

obj/client.o: src/client.c src/history.c src/argsList.c src/communication.c
	gcc -c -g src/client.c src/history.c src/argsList.c src/communication.c
	mv *.o obj/

bin/server: obj/server.o obj/hashServer.o obj/communication.o obj/http.o obj/directories.o
	gcc -o bin/server obj/server.o obj/hashServer.o obj/communication.o obj/http.o obj/directories.o

obj/server.o: src/server.c src/hashServer.c src/communication.c src/http.c src/directories.c
	gcc -c -g -D_GNU_SOURCE src/server.c src/hashServer.c src/communication.c src/http.c src/directories.c
	mv *.o obj/

clean:
	clear
	rm -r Proxy/ obj/*.o bin/*
