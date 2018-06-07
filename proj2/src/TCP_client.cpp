
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include "TCP.h"
using namespace std;

TCP_client::TCP_client(string IP)
{
	seq_number = 0;
	ack_number = 0;
	servIP = IP;
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	memset((char *)&hostaddr, 0, sizeof(hostaddr));
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	hostaddr.sin_port = htons(0);
	/* bind it to all local addresses and pick any port number */
	if (bind(fd, (struct sockaddr *)&hostaddr, sizeof(hostaddr)) < 0) 
		fatal_error("bind failed");

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	remaddr.sin_addr.s_addr = inet_addr(IP.c_str());


}

void TCP_client::initiate_connection()
{	
	/*SYN-ACK-SYNACK*/
	tcp_packet pkt;
	make_packet(pkt, SYN, "");
	/*Tell the server our first sequence number is 0*/
	pkt.seq_num = seq_number;
	transmit_pkt(pkt);
	seq_number++;
	recv_pkt(pkt);

	if(pkt.ack_num != seq_number)
		fatal_error("ACK number not synchronized. Terminating connection");
	
	/*Set our ack number from the server's sequence number+1
	This is done in recv_pkt*/
	
    make_packet(pkt, ACK, "");
	transmit_pkt(pkt);

}

void TCP_client::request_file(string file_name, char *ext)
{
	printf("\n***Requesting file %s***\n", file_name.c_str());
	tcp_packet pkt;
	vector<tcp_packet> received_packets;
	make_packet(pkt, DATA, file_name);

	transmit_pkt(pkt);
	/*Wait for ACK*/
	recv_pkt(pkt);
	/*Do something on ACK, not sure yet*/

	/*Receive file*/
	printf("\n***Receiving file***\n");
	uint8_t fin_flag = 0x11;

	ofstream outfile;
	string outname = "received_file.";
	outname.append(ext);
  	outfile.open(outname, ios::binary);

	recv_pkt(pkt);
	received_packets.push_back(pkt);
	int o = 0, p = 1;
	
	

	
	while ((fin_flag & pkt.flags) != 0x11)
	{
		recv_pkt(pkt);
		received_packets.push_back(pkt);
	}
	printf("Received EOF\n");
	/*Organize the packets by sequence number*/

	vector<tcp_packet>::iterator it = received_packets.begin();


	/*Find the file's lowest sequence number, 
	in case the first packet is sent out of order*/
	int first_seq_num = it->seq_num;
	it++;
	for(; it != received_packets.end(); it++)
		if(it->seq_num < first_seq_num)
			first_seq_num = it->seq_num;


	/*Write to the file in the right order*/
	for(int k = 0; k < received_packets.size(); k++)
		for(it = received_packets.begin(); it != received_packets.end(); it++)
			if(it->seq_num == first_seq_num)
			{	
				outfile.write(it->data, it->len_data);
				first_seq_num++;
				break;
			}
	outfile.close();


}


void TCP_client::teardown()
{
		/*Wait for FIN*/
	cout << "\n***Tearing down***\n";
	tcp_packet pkt, ack;
	recv_pkt(pkt);
	/*Send ACK*/
	make_packet(ack, ACK, "");
	ack.ack_num = pkt.seq_num+1;
	transmit_pkt(pkt);
	/*Send FIN*/
	make_packet(pkt, FIN, "");
	transmit_pkt(pkt);
	/*Wait for ACK*/
	recv_pkt(pkt);
	close(fd);
}











