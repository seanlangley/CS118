#include <stdlib.h>
#include <stdio.h>
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
#include <vector>
#include <pthread.h>

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
	memset((char *)&remaddr, 0, sizeof(remaddr));
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//hostaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	hostaddr.sin_port = htons(SERVICE_PORT);
	if (bind(fd, (struct sockaddr *)&hostaddr, sizeof(hostaddr)) < 0) {
	fatal_error("bind failed");
	
	}
}



void TCP_server::initiate_connection()
{
	cout << "Waiting for connection on " << SERVICE_PORT << endl;
	tcp_packet pkt;
	/*Wait for SYN*/
	recv_pkt(pkt);
	/*SYNchronize ack number to received seq_num+1*/
	ack_number = pkt.seq_num+1;
	/*Send the SYN-ACK, use initual sequence number 0*/
	sequence_number = 0;
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
	cout << "\n***Waiting for file request***\n";
	recv_pkt(pkt);
	file_name = pkt.data;
	return file_name;
}



void *receive_acks(void *arg)
{	
	struct thread_args *args = (struct thread_args*) arg;
	TCP_server *serv = (TCP_server *) args->arg1;
	size_t num_packs = (size_t) args->arg2;
	tcp_packet ack;	

	for(int k = 0; k < num_packs; k++)
		serv->recv_pkt(ack);
	return NULL;
}




void TCP_server::send_file(vector<tcp_packet> file_pkts)
{
	tcp_packet out_pkt;
	cout << "\n***Sending file***\n";

	/*Create a thread to receive ACKS*/
	struct thread_args args;
	args.arg1 = this;
	args.arg2 = file_pkts.size();
	pthread_t thread;
	pthread_create(&thread, NULL, receive_acks, &args);

	vector<tcp_packet>::iterator it = file_pkts.begin();
	while(it != file_pkts.end())
	{
		out_pkt = *it;
		transmit_pkt(out_pkt);
		it++;
	}
	make_packet(out_pkt, END, "");
	transmit_pkt(out_pkt);
	pthread_join(thread, NULL);


}



void TCP_server::teardown()
{

	/*Send FIN*/
	cout << "\n***Tearing down***\n";
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



std::vector<tcp_packet> TCP_server::parse_file(string file)
{
	unsigned long file_size = file.size();
	int offset = 0;
	int data_size = 1010;
	vector<tcp_packet> file_pkts;
	tcp_packet pkt;
	while (file_size > 0)
	{
		if (file_size > data_size)
		{
			make_packet(pkt, DATA, file.substr(offset*data_size, data_size));
			file_pkts.push_back(pkt);
			
			file_size -= data_size;
			offset++;
		}
		else
		{

			make_packet(pkt, DATA, file.substr(offset*data_size, file_size));
			file_pkts.push_back(pkt);
			file_size = 0;
		}
	}
	return file_pkts;
}










