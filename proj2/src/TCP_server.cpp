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
#include <ctime>
#include <chrono>
#include <thread>
#include <sched.h>
#include <signal.h>

#include "TCP.h"


TCP_server::TCP_server()

{	
	seq_number = 10;
	ack_number = 0;
	all_acked = false;
	window_size = 10;
	base = 0;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fatal_error("cannot create socket\n");

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&hostaddr, 0, sizeof(hostaddr));
	memset((char *)&remaddr, 0, sizeof(remaddr));
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//hostaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	hostaddr.sin_port = htons(SERVICE_PORT);
	bind(fd, (struct sockaddr *)&hostaddr, sizeof(hostaddr));

	
	
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
	seq_number = 0;
	make_packet(pkt, SYNACK, "");
	/*send the ACK*/
	transmit_pkt(pkt);
	seq_number++;
	/*Wait for SYN-ACK*/
	recv_pkt(pkt);
	if(pkt.ack_num != seq_number)
		fatal_error("ACK number not synchronized. Terminating connection");
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
	{
		serv->recv_pkt(ack);
		serv->set_acks(ack.ack_num);
		serv->set_base(ack.ack_num);
		if(ack.ack_num -1 == serv->get_num_packets())
			serv->set_all_acked();
	}
	return NULL;
}


void *manage_timeouts(void *arg)
{
	/*Go through the entire list of packets
	If a packet wasn't acked, check its transmission time
	If its time is greater than RTO, retransmit and
	reset the transmission time*/
	ms RTO = ms(1000);
	while(1)
	{	
		this_thread::sleep_for(chrono::milliseconds(RTO));
		TCP_server *serv = (TCP_server *) arg;
		std::vector<packet_meta>::iterator it;
		std::vector<packet_meta> *server_meta_data = serv->get_packet_meta();

		for(it = server_meta_data->begin(); it != server_meta_data->end(); it++)
		{
			if(it->was_acked == true)
				continue;
			Time::time_point now = Time::now();
			fsec fs = now - it->time_sent;
   			ms d = std::chrono::duration_cast<ms>(fs);
			if(d.count()  > RTO.count())
			{
				tcp_packet pkt = *(it->pkt);
				pkt.flags |= RETRANS;
				it->time_sent = Time::now();
				serv->transmit_pkt(pkt);
			}
		}
	}	
	return NULL;
}

void TCP_server::send_file(std::vector<tcp_packet> file_pkts)
{

	std::vector<tcp_packet>::iterator it = file_pkts.begin();
	base = it->seq_num;
	for(; it != file_pkts.end(); it++)
		packet_meta_data.push_back(make_meta(&(*it)));

	tcp_packet out_pkt;
	cout << "\n***Sending file***\n";

	/*Create a thread to receive ACKS*/
	struct thread_args args;
	args.arg1 = this;
	args.arg2 = file_pkts.size();

	pthread_t ack_thread;
	pthread_t timeout_thread;
	pthread_create(&ack_thread, NULL, receive_acks, &args);
	pthread_create(&timeout_thread, NULL, manage_timeouts, this);


	for(int k = 0; k < file_pkts.size(); k++)
	{
		out_pkt = file_pkts[k];
		/*If the packet is in the window, send the packet*/
		if(out_pkt.seq_num < base + window_size)
		{
			packet_meta_data[k].time_sent = Time::now();
			transmit_pkt(out_pkt);
		}
		/*Otherwise, wait until the base moves forward*/
		else
			while(true)
			{ 
				sched_yield();
				if(out_pkt.seq_num < base + window_size)
				{
					packet_meta_data[k].time_sent = Time::now();
					transmit_pkt(out_pkt);
					break;
				}
			}
		
	}

	/*Wait for all packets to be acked*/	
	while(all_acked == false)
		this_thread::sleep_for(chrono::milliseconds(250));
		
	make_packet(out_pkt, END, "");
	transmit_pkt(out_pkt);
	pthread_join(ack_thread, NULL);
	pthread_kill(timeout_thread, 0);

}


void TCP_server::teardown()
{

	/*Send FIN*/
	seq_number++;
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



std::vector<tcp_packet> TCP_server::parse_file(std::string file)
{
	unsigned long file_size = file.size();
	int offset = 0;
	int data_size = 1010;
	std::vector<tcp_packet> file_pkts;
	tcp_packet pkt;


	
	std::vector<tcp_packet>::iterator it;
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
	num_packets = file_pkts.size();
	return file_pkts;
}










