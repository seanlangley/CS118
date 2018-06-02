#ifndef rdt_H
#define rdt_H

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

struct tcp_packet{
  uint32_t seq_num;
  char data[MSS];
  
};



class rdt{
public:
	rdt();
	rdt(std::string IP);
	void send_init();
	void recv_init();
	std::string rdt_send(std::string data);
	void rdt_recv(tcp_packet &pkt);
	void udt_send(tcp_packet pkt);
	tcp_packet make_pkt(int seqnum, std::string data);
	inline uint32_t getnextseqnum(){return nextseqnum;}
	inline uint8_t getbase(){return base;}
private:
	int fd;
	socklen_t addrlen;
	struct sockaddr_in remaddr, hostaddr;
	uint32_t expectedseqnum, nextseqnum, base;
	uint8_t N;
	clock_t tmer;
	std::vector<tcp_packet> sndpkt;

};

#endif