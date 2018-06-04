CC=g++
CFLAGS = -g -Wall -Wextra -pthread
all: receiver sender

receiver: receiver.o
	$(CC) -o receiver $(CFLAGS) receiver.o
sender: sender.o
	$(CC) -o sender $(CFLAGS) sender.o

receiver.o: receiver.cpp
	$(CC) -c receiver.cpp

sender.o: sender.cpp
	$(CC) -c sender.cpp

clean:
	rm *.o receiver sender