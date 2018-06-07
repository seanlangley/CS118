
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



int main(int argc, char *argv[])
{
	if(argc < 2)
		fatal_error("No file name");
	std::string file_name = argv[1];
	/*Get the file extension*/
	strtok(argv[1], ".");
	char *ext = strtok(NULL, "\0");
	std::string servIP = "127.0.0.1";	
	TCP_client tcp(servIP);
	tcp.initiate_connection();
	tcp.request_file(file_name, ext);
	tcp.teardown();
}
