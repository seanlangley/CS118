
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

#include "TCP.h"
using namespace std;


int main(int argc, char *argv[])
{
	if(argc < 2)
		fatal_error("No file name");
	string file_name = argv[1];
	string servIP = "127.0.0.1";	/* change this to use a different server */
	TCP_client tcp(servIP);
	tcp.initiate_connection();
	string file = tcp.request_file(file_name);
	tcp.teardown();
	return 0;
}
