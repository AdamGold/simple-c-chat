CC=gcc

all:
	$(CC) -Wall server.c utilities.c -O2 -pthread -o csr
	$(CC) -Wall client.c utilities.c -O2 -pthread -o clr
