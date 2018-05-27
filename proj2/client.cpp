
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
#include "TCP.h"
using namespace std;


int main(void)
{
	string servIP = "127.0.0.1";	/* change this to use a different server */
	TCP_client tcp(servIP);
	tcp.initiate_connection();
	
	string file_name = "file.txt";
	string file = tcp.request_file(file_name);
	cout << file << endl;
	cout << "Tearing down...\n";
	tcp.teardown();


	return 0;
}
