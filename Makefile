# Note, you must change the port to your unique port number (between 1000 and 2^16-1) based on your Student ID
PORT = 3233
PROGRAMS = server client
S = -std=c99 -ggdb

both: $(PROGRAMS)

server: server.o Timer.o
	gcc $S server.c Timer.o -o server

server.o: server.c Timer.h Timer.c
	gcc -c server.c

client: client.o Timer.o 
	gcc $S client.o -lpthread Timer.o -o client

client.o: client.c Timer.h Timer.c
	gcc -c client.c

Timer.o: Timer.h Timer.c
	gcc -c Timer.c


test: both
	./server $(PORT) &
	echo Starting client
	./client 127.0.0.1 $(PORT)
	ls -lr Thread_*
	du

clean:
	/bin/rm -rf $(PROGRAMS) Thread_*
	/bin/rm *.o 
