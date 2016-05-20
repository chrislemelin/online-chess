CC=gcc
CFLAGS= -std=c99
SCHESS = server/chess/

server: server/server

client: client/client

server/server: $(SCHESS)board.o $(SCHESS)display.o $(SCHESS)piece.o server/server.c
	$(CC) $(CFLAGS) server/server.c $(SCHESS)board.o $(SCHESS)piece.o $(SCHESS)display.o -o server/server

$(SCHESS)board.o: $(SCHESS)board.c
	$(CC) $(SCHESS)board.c -c -o $@;
#	mv board.o $(SCHESS)board.o

$(SCHESS)display.o: $(SCHESS)display.c
	$(CC) $(SCHESS)display.c -c -o $@;
#	mv display.o $(SCHESS)display.o

$(SCHESS)piece.o: $(SCHESS)display.c
	$(CC) $(SCHESS)piece.c -c -o $@;
#	mv piece.o $(SCHESS)piece.o

client/client: client/clientview.o client/display.o client/client.c
	$(CC) client/client.c client/clientview.o client/display.o -o client/client

client/clientview.o: client/clientview.c
	$(CC) client/clientview.c -c -o $@

client/display.o: client/display.c
	$(CC) client/display.c -c -o $@

