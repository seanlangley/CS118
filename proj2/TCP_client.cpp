
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include "TCP.h"
using namespace std;

TCP_client::TCP_client(string IP)
{
	sequence_number = 0;
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
	tcp_packet pkt;
	/*Send the SYN*/
	make_packet(pkt, SYN, "");
	sequence_number++;
	send_pkt(pkt);
	/*Receive the ACK, set the ACKNUM*/
	recv_pkt(pkt);
    /*Send the SYN-ACK*/
    /*ACKs don't increase the seq_num*/
    make_packet(pkt, ACK, "");
	send_pkt(pkt);

}

void TCP_client::request_file(string file_name)
{
	cout << "Requesting file " << file_name << endl;
	tcp_packet pkt;
	make_packet(pkt, DATA, file_name);
	printf("pkt.data is %s\n", pkt.data);
	send_pkt(pkt);
	/*Wait for ACK*/
	recv_pkt(pkt);
}

void TCP_client::recv_file()
{
	tcp_packet pkt;
	recv_pkt(pkt);
	cout << pkt.data << endl;
}












