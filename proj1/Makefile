CC=g++
CPPFLAGS=-g -Wall -Wextra
USERID=504661838


default: server

server: clean server.o Sock.o
	$(CC) -o server $(CPPFLAGS) server.o Sock.o
server.o: server.cpp Sock.h Sock.cpp
	$(CC) -c server.cpp
Sock.o: Sock.h
	$(CC) -c Sock.cpp

clean:
	rm -rf server *.o

dist:
	tar zcvf 104584663.tar.gz cat.gif doge.jpg Makefile not_found.html README server.cpp Sock.cpp Sock.h test.html report.pdf