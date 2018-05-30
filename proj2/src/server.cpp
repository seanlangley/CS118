
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "TCP.h"
using namespace std;

int main(int argc, char **argv)
{
	TCP_server tcp;
	tcp.initiate_connection();
	string file_name = tcp.get_file_request();
	string file;
	getfile(file, file_name);
	// unsigned long file_size = file.size();
	// printf("File size: %lu\n", file_size);
	tcp.send_file(file);
	
	tcp.teardown();

	
}
