
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
	if(argc < 4)
		cli_usage();

	std::string file_name = argv[3];
	unsigned int portno = atoi(argv[2]);
	string servIP = argv[1];

	/*Get the file extension*/
	strtok(argv[3], ".");
	char *ext = strtok(NULL, "\0");
	TCP_client tcp(servIP, portno);
	tcp.initiate_connection();
	tcp.request_file(file_name, ext);
	tcp.teardown();
}
