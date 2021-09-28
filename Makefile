all: bin/client bin/server

bin/client: obj/client.o obj/history.o obj/argsList.o obj/communication.o obj/directories.o
	gcc -o bin/client obj/client.o obj/history.o obj/argsList.o obj/communication.o obj/directories.o -pthread

obj/client.o: src/client.c src/history.c src/argsList.c src/communication.c
	gcc -c -g -pthread -D_GNU_SOURCE src/client.c src/history.c src/argsList.c src/communication.c src/directories.c
	mv *.o obj/

bin/server: obj/server.o obj/hashServer.o obj/communication.o obj/http.o obj/directories.o
	gcc -o bin/server obj/server.o obj/hashServer.o obj/communication.o obj/http.o obj/directories.o -pthread

obj/server.o: src/server.c src/hashServer.c src/communication.c src/http.c src/directories.c
	gcc -c -g -pthread -D_GNU_SOURCE src/server.c src/hashServer.c src/communication.c src/http.c src/directories.c
	mv *.o obj/

clean:
	clear
	rm clients/client1/* clients/client2/* clients/client3/* proxy/* infoPage/* obj/* bin/*
