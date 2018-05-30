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
#include <fstream> 
#include <iostream>
#include "TCP.h"
using namespace std;

TCP_server::TCP_server()
{
	sequence_number = 10;
	ack_number = 0;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fatal_error("cannot create socket\n");

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&hostaddr, 0, sizeof(hostaddr));
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
	memset(&pkt, 0, sizeof(pkt));
	transmit_pkt(pkt);

	return file_name;
}

void TCP_server::send_file(string file)
{
	tcp_packet pkt;
	cout << "Sending file...\n";
	unsigned long file_size = file.size();
	//Can send 1015B of data per packet
	int offset = 0;
	int data_size = 1010;
	// printf("file size: %lu\n", file_size);
	// char sub[1010];



	while (file_size > 0)
	{
		// memset(sub, 0, sizeof(sub));
		if (file_size > data_size)
		{
			// 	char sub[1010];
			// for (int i = 0; offset < data_size; offset++, i++)
			// {
			// 	// printf("%c\n", pkt.data[offset]);
			// 	// sub[i] = pkt.data[offset];
			// 	sprintf(sub, "%02hhX", pkt.data[offset]);
			// }
			// printf("%s\n", sub);
			// make_packet(pkt, DATA, sub);
			make_packet(pkt, DATA, file.substr(offset*data_size, data_size));


			transmit_pkt(pkt);
			file_size -= data_size;
			offset++;
		}
		else
		{
			// 	char sub[1010];
			// for (int i = 0; offset < data_size; offset++, i++)
			// {
			// 	sub[i] = pkt.data[offset];
			// 	// sprintf(sub, "%02hhX", pkt.data[offset]);
			// }
			// make_packet(pkt, DATA, sub);
			make_packet(pkt, DATA, file.substr(offset*data_size, file_size));
			transmit_pkt(pkt);
			make_packet(pkt, END, "");
			transmit_pkt(pkt);
			file_size = 0;
		}
	}
	/*Wait for ACK*/
	recv_pkt(pkt);
}

void TCP_server::teardown()
{

		/*Send FIN*/
	tcp_packet pkt;
	make_packet(pkt, FIN, "");
	transmit_pkt(pkt);
	/*Wait for ACK*/
	recv_pkt(pkt);
	/*Wait for FIN*/
	recv_pkt(pkt);
	/*Send ACK*/
	make_packet(pkt, ACK, "");
	transmit_pkt(pkt);
	close(fd);
}







			// sub = file.c_str() + (offset * data_size);		//added
			
			// char target[2000];
			// strncpy(target, sub, data_size);
			// target[1000] = '\0'; // IMPORTANT!


			// sub = file.c_str() + (offset * data_size);		//added

			// char target[2000];
			// strncpy(target, sub, data_size);
			// target[1000] = '\0'; // IMPORTANT!










