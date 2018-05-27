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
#include <string.h>
#include "TCP.h"
using namespace std;

TCP_server::TCP_server()
{
	sequence_number = 10;
	ack_number = 0;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fatal_error("cannot create socket\n");

	/* bind the socket to any valid IP address and a specific port */

	//memset((char *)&hostaddr, 0, sizeof(hostaddr));
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	hostaddr.sin_port = htons(SERVICE_PORT);
	if (bind(fd, (struct sockaddr *)&hostaddr, sizeof(hostaddr)) < 0) {
	fatal_error("bind failed");
	
	}
}


void TCP_server::initiate_connection()
{
	cout << "Waiting for SYN on port " << SERVICE_PORT << endl;
	tcp_packet pkt;
	/*Wait for SYN*/
	recv_pkt(pkt);
	/*Send the SYN-ACK*/
	make_packet(pkt, SYNACK, "");
	/*send the ACK*/
	transmit_pkt(pkt);
	/*Wait for SYN-ACK*/
	recv_pkt(pkt);
}

string TCP_server::get_file_request()
{
	tcp_packet pkt;
	string file_name;
	cout << "Waiting for file request\n";
	recv_pkt(pkt);
	file_name = pkt.data;

	/*Send ACK*/
	//memset(&pkt, 0, sizeof(pkt));
	transmit_pkt(pkt);

	return file_name;
}














