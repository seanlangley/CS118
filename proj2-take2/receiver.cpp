
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "helpers.h"
#include "rdt.h"
using namespace std;


int main(int argc, char *argv[])
{
	
	string file_name = "input.txt";
	string servIP = "127.0.0.1";	/* change this to use a different server */
	rdt receiver(servIP);
	receiver.recv_init();
	tcp_packet pkt;
	receiver.rdt_recv(pkt);
	cout << pkt.data << endl;
	return 0;
}
