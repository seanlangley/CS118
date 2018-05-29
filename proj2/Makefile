CC=g++
CFLAGS = -g -Wall -Wextra


all: client server clean.o

client: client.o TCP_client.o TCP.o
	$(CC) -o client $(CFLAGS) client.o TCP_client.o TCP.o

TCP_client.o: TCP_client.cpp TCP.h helpers.h 
	$(CC) -c TCP_client.cpp

client.o: client.cpp TCP.h TCP_client.cpp helpers.h 
	$(CC) -c client.cpp

server: server.o TCP_server.o TCP.o
	$(CC) -o server $(CFLAGS) server.o TCP_server.o TCP.o

TCP_server.o: TCP_server.cpp TCP.h helpers.h
	$(CC) -c TCP_server.cpp

server.o: server.cpp TCP.h TCP_server.cpp helpers.h
	$(CC) -c server.cpp


TCP.o: TCP.cpp TCP.h helpers.h
	$(CC) -c TCP.cpp



clean:
	rm -f client server *.o 
clean.o:
	rm -f *.o