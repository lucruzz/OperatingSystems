bin/client: obj/client.o
	gcc -o bin/client obj/client.o obj/history.o obj/argsList.o obj/communication.o
	bin/server
	bin/client

obj/client.o: src/client.c
	gcc -c -g src/client.c src/history.c src/argsList.c src/communication.c
	mv *.o obj/

bin/server: obj/server.o
	gcc -o bin/server obj/server.o obj/communication.o

obj/server.o: src/server.c src/communication.c
	gcc -c -g src/server.c src/communication.c
	mv *.o obj/

clean:
	clear
	rm obj/*.o
	rm bin/*
