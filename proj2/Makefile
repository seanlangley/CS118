CC=g++
CPPFLAGS=-g -Wall -Wextra
USERID=504661838

default: client server

client: clean client.o 
	$(CC) -o client $(CPPFLAGS) client.o 
client.o: client.cpp helpers.h
	$(CC) -c client.cpp
server: clean server.o 
	$(CC) -o server $(CPPFLAGS) server.o 
server.o: server.cpp  helpers.h
	$(CC) -c server.cpp


clean:
	rm -rf client server *.o

