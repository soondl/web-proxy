SRC1 = proxy_server.c
SRC2 = main.c

EXEC = proxy_server

CFLAGS = -g

CC = gcc

all:
	$(CC) -o $(EXEC) $(SRC1) $(SRC2) -lcrypto -lpthread

clean:
	rm -rf $(EXEC)
