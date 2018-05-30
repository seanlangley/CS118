#ifndef TCP_H
#define TCP_H

#include "helpers.h"
#define BUFLEN 1024

#define SERVICE_PORT  21235
#define BUFSIZE 2048
#define DATA 0x0
#define SYN 0x2
#define ACK 0x10
#define SYNACK 0x12
#define FIN 0x1
#define END 0x11

struct tcp_packet{
  uint32_t seq_num;
  uint32_t ack_num;
  /*ACK=3, SYN=6, FIN=7, etc...*/
  uint8_t flags;
  /*MAX data size is 1024 - header size*/
  // char data[1015];
  uint32_t len_data;
  char data[1011];
  
};


class TCP{
public:
	TCP(){return;}
	virtual ~TCP(){return;}
	void make_packet(tcp_packet &pkt, uint32_t flags, string data);			//string data
	void transmit_pkt(tcp_packet &pkt);
	void recv_pkt(tcp_packet &pkt);
	void resize(char * arr);
protected:
	/*Need 30720 sequence numbers, use uint_16 = 65535*/
	uint16_t sequence_number;
	uint16_t ack_number;
	int fd;
	socklen_t addrlen;
	struct sockaddr_in remaddr, hostaddr;
};

class TCP_client : public TCP{
public:
	TCP_client(std::string IP);
	void initiate_connection();
	string request_file(std::string file_name);
	void teardown();
private:
	string servIP;
};

class TCP_server: public TCP{
public:
	TCP_server();
	void initiate_connection();
	string get_file_request();
	void send_file(string file);
	void teardown();
	static void *receive_acks(void *args);
};

#endif