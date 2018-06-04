#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include "rdt.h"
#include "helpers.h"
#include <iostream>
#include <thread>
using namespace std;

tcp_packet rdt::make_pkt(int seqnum, string data)
{
  tcp_packet pkt;
  pkt.seq_num = seqnum;
  strcpy(pkt.data, data.c_str());
  return pkt;

}

rdt::rdt()
{
	base = 0;
	nextseqnum = 0;
	N = 5;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fatal_error("cannot create socket\n");

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&hostaddr, 0, sizeof(hostaddr));
	memset((char *)&remaddr, 0, sizeof(remaddr));
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	hostaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&hostaddr, sizeof(hostaddr)) < 0) 
		fatal_error("bind failed");
	
}
/*Runs on receiver*/
rdt::rdt(string servIP)
{
	expectedseqnum = 0;
	string IP = servIP;

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

void rdt::send_init()
{
	cout << "Waiting for connection on port " << SERVICE_PORT << endl;
	tcp_packet pkt;
	rdt_recv(pkt);
	cout << "Connection established\n";
}

void rdt::recv_init()
{
	cout << "Connecting to port " << SERVICE_PORT << endl;
	string data = "SYN";
	tcp_packet pkt = make_pkt(nextseqnum, data);
	udt_send(pkt);
}

void rdt::rdt_recv(tcp_packet &pkt)
{
	memset(&pkt, 0, sizeof(pkt));
	addrlen = sizeof(remaddr);
	int recvlen = recvfrom(fd, &pkt, sizeof(pkt), 0, 
	(struct sockaddr *)&remaddr, &addrlen);
	if(recvlen < 0)
		perror("recvfrom");
	//if corrupt: base = 0, nextseqnum = 0
	//if not corrupt:
	base = pkt.seq_num + 1;
	if (base == nextseqnum)
	{
		stop = clock();
		printf("yes");
	}
	else
	{
		start = clock();
	}
}

string rdt::rdt_send(string data)
{ 
	if(nextseqnum < base+N)
	{
		tcp_packet pkt = make_pkt(nextseqnum, data);
		sndpkt.push_back(pkt);
		udt_send(sndpkt[nextseqnum]);

		/*Add timer here*/
		if(base == nextseqnum)
		{
			start = clock();
		}
		nextseqnum++;
		return "";
	}
	else
		return data;
}


void rdt::udt_send(tcp_packet pkt)
{
	addrlen = sizeof(remaddr);
	int bytes_sent;
	bytes_sent = sendto(fd, &pkt, sizeof(pkt), 0, 
    (struct sockaddr *)&remaddr, addrlen);
	if(bytes_sent <= 0)
		fatal_error("sendto");
}