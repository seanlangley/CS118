#ifndef TCP_H
#define TCP_H

#include "helpers.h"
#include <vector>
#include <chrono>
#define MSS 1024

#define SERVICE_PORT  21235
#define BUFSIZE 2048
#define DATA 0x0
#define SYN 0x02
#define ACK 0x10
#define SYNACK 0x12
#define RETRANS 0x80
#define FIN 0x1
#define END 0x11

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;


struct tcp_packet{
  uint32_t seq_num;
  uint32_t ack_num;
  /*ACK=3, SYN=6, FIN=7, RETRANS = 1*/
  uint8_t flags;
  uint32_t len_data;
  char data[1011];
  
};

struct packet_meta{
	struct tcp_packet *pkt;
	Time::time_point time_sent;
	bool was_acked;
};


class TCP_server;

struct thread_args{
	TCP_server *arg1;
	size_t arg2;
};



class TCP{
public:
	TCP(){return;}
	virtual ~TCP(){return;}
	void make_packet(tcp_packet &pkt, uint32_t flags, string data);	
	packet_meta make_meta(tcp_packet *pkt);
	void transmit_pkt(tcp_packet &pkt);
	void recv_pkt(tcp_packet &pkt);
	void resize(char * arr);
	void print_addr_info();
	void set_acks(uint32_t acknum);

	inline int get_seq_num(){
		return seq_number;
	}
	inline std::vector<packet_meta> *get_packet_meta(){
		return &packet_meta_data;
	}
	inline void set_all_acked(){
		all_acked = true;
	}
	inline void set_base(int new_base){
		base = new_base;
	}
protected:
	/*Need 30720 sequence numbers, use uint_16 = 65535*/
	uint16_t seq_number;
	uint16_t ack_number;
	int fd;
	socklen_t addrlen;
	struct sockaddr_in remaddr, hostaddr;
	std::vector<packet_meta> packet_meta_data;



	bool all_acked;
	unsigned int window_size;
	unsigned int base;
	
};

class TCP_client : public TCP{
public:
	TCP_client(std::string IP);
	void initiate_connection();
	void request_file(std::string file_name, char *ext);
	void teardown();
private:
	string servIP;

};

class TCP_server: public TCP{
public:
	TCP_server();
	void initiate_connection();
	string get_file_request();
	void send_file(vector<tcp_packet> file_pkts);
	void teardown();
	vector<tcp_packet> parse_file(string file);

	inline uint32_t get_num_packets()
			{return num_packets;}
private:
	clock_t timer;
	uint32_t num_packets;


};

#endif