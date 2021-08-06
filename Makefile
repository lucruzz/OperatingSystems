bin/client: obj/client.o
	gcc -o bin/client obj/client.o obj/history.o obj/argsList.o
	bin/client

obj/client.o: src/client.c
	gcc -c -g src/client.c src/history.c src/argsList.c
	mv *.o obj/

clean:
	clear
	rm obj/*.o
	rm bin/*
