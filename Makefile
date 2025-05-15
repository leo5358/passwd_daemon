CC = gcc
CFLAGS = -Wall

LIBS = -lpam -lpam_misc

all: daemon client

daemon: daemon.c
	$(CC) $(CFLAGS) daemon.c -o daemon $(LIBS)

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f daemon client
