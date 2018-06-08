
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
#include <vector>
#include "TCP.h"
using namespace std;

int main(int argc, char **argv)
{
	if(argc < 2)
		serv_usage();

	TCP_server tcp(atoi(argv[1]));
	tcp.initiate_connection();
	std::string file_name = tcp.get_file_request();
	std::string file;
	getfile(file, file_name);
  	
	std::vector<tcp_packet> file_pkts;
	/*Contains all the packets ready to send*/
	file_pkts = tcp.parse_file(file);
	std::vector<tcp_packet>::iterator it = file_pkts.begin();

	tcp.send_file(file_pkts);
	tcp.teardown();
	
}
