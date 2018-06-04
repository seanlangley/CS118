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
#include "helpers.h"
using namespace std;

#define BUFLEN 1024
#define SERVICE_PORT  21235
#define BUFSIZE 2048
#define DATA 0x0
#define SYN 0x2
#define ACK 0x10
#define SYNACK 0x12
#define FIN 0x1
#define END 0x11
#define MSS 1020

pthread_mutex_t start_mutex;
pthread_mutex_t pkt_mutex;
double to_ms = CLOCKS_PER_SEC/1000;
void* status;

struct tcp_packet
{
  uint32_t seq_num;
  char data[MSS];
};

int fd;
socklen_t addrlen;
struct sockaddr_in remaddr, hostaddr;
uint32_t expectedseqnum, nextseqnum, base;
uint8_t N;
clock_t start;
clock_t stop;
std::vector<tcp_packet> sndpkt;

tcp_packet make_pkt(int seqnum, std::string data)
{
	tcp_packet pkt;
	pkt.seq_num = seqnum;
  	strcpy(pkt.data, data.c_str());
  	return pkt;
}

void initialize()
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

void rdt_recv(tcp_packet &pkt)
{
	memset(&pkt, 0, sizeof(pkt));
	addrlen = sizeof(remaddr);
	int recvlen = recvfrom(fd, &pkt, sizeof(pkt), 0, 
	(struct sockaddr *)&remaddr, &addrlen);
	if(recvlen < 0)
		perror("recvfrom");
	//if corrupt: base = 0, nextseqnum = 0
	//if not corrupt:
	pthread_mutex_lock(&start_mutex);
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
	pthread_mutex_unlock(&start_mutex);
}

void send_init()
{
	cout << "Waiting for connection on port " << SERVICE_PORT << endl;
	tcp_packet pkt;
	rdt_recv(pkt);
	cout << "Connection established\n";
}

void udt_send(tcp_packet pkt)
{
	addrlen = sizeof(remaddr);
	int bytes_sent;
	bytes_sent = sendto(fd, &pkt, sizeof(pkt), 0, 
    (struct sockaddr *)&remaddr, addrlen);
	if(bytes_sent <= 0)
		fatal_error("sendto");
}

void recv_init()
{
	cout << "Connecting to port " << SERVICE_PORT << endl;
	string data = "SYN";
	pthread_mutex_lock(&start_mutex);
	tcp_packet pkt = make_pkt(nextseqnum, data);
	pthread_mutex_unlock(&start_mutex);
	udt_send(pkt);
}

std::string rdt_send(std::string data)
{
	pthread_mutex_lock(&start_mutex);
	if(nextseqnum < base+N)
	{
		tcp_packet pkt = make_pkt(nextseqnum, data);
		sndpkt.push_back(pkt);
		udt_send(sndpkt[nextseqnum]);

		/*Add timer here*/
		if(base == nextseqnum)
		{
			// pthread_mutex_lock(&start_mutex);
			start = clock();
			// pthread_mutex_unlock(&start_mutex);
		}
		nextseqnum++;
		pthread_mutex_unlock(&start_mutex);
		return "";
	}
	else
		return data;
}

void* check_if_timeout(void*)
{
	while(1)
	{
		pthread_mutex_lock(&start_mutex);
		if (((clock() - start)/to_ms) > 500)
		{
			start = clock();
			//timeout actions
			for (int i = base; i < nextseqnum; i++)
			{
				udt_send(sndpkt[i]);
			}
			pthread_mutex_unlock(&start_mutex);
		}
		else
		{
			pthread_mutex_unlock(&start_mutex);
			usleep(100);
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	start = clock();

	pthread_t for_timeout;
	int rc = pthread_create(&for_timeout, NULL, check_if_timeout, NULL);

	// Timeout Initialization
	// pthread_mutex_lock(&start_mutex);

	// pthread_mutex_lock(&stop_mutex);
	// pthread_mutex_unlock(&start_mutex);
	// pthread_mutex_unlock(&stop_mutex);

	initialize();
	send_init();
	string file_name = "file.txt";
	string file;
	getfile(file, file_name);
	uint32_t offset = 0;
	
	string data = "";
	/*One packet case*/
	if(file.size() < MSS)
	{
		data = rdt_send(file);
		while(data == file)
			data = rdt_send(file);

	}
	
	else
	{
		while(offset+MSS != file.size())
		{
			data  = file.substr(offset, MSS);
			offset += MSS;
			string confirm = data;
			while(confirm == data)
				confirm = rdt_send(data);
		}
	}

	pthread_kill(for_timeout, SIGINT);
	pthread_join(for_timeout, &status);
	pthread_exit(NULL);
}
