#ifndef TCP_H
#define TCP_H

#include "helpers.h"
//#include <vector>
#define BUFLEN 1024

#define SERVICE_PORT  21234
#define BUFSIZE 2048
#define DATA 0x0
#define SYN 0x2
#define ACK 0x10
#define SYNACK 0x12
#define FIN 0x1
/*MSS is 1024 - header size*/
#define MSS 1015


struct tcp_packet{
  uint32_t seq_num;
  uint32_t ack_num;
  /*ACK=3, SYN=6, FIN=7, etc...*/
  uint8_t flags;
  char data[MSS];
};


class TCP{
public:
	TCP(){return;}
	virtual ~TCP();
	void make_packet(tcp_packet &pkt, uint32_t flags, string data);
	void send_pkt(tcp_packet &pkt);
	void recv_pkt(tcp_packet &pkt);
protected:
	int sequence_number;
	int ack_number;
	int fd;
	socklen_t addrlen;
	struct sockaddr_in remaddr, hostaddr;
};

class TCP_client : public TCP{
public:
	TCP_client(std::string IP);
	void initiate_connection();
	void request_file(std::string file_name);
	void recv_file();
private:
	string servIP;
};

class TCP_server: public TCP{
public:
	TCP_server();
	~TCP_server();
	void initiate_connection();
	string get_file_request();
	void parse_file(string file);
	void send_file();
private:
	std::vector<tcp_packet> *file_pkts;

};

#endif