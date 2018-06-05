#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include "TCP.h"
using namespace std;

void TCP::make_packet(tcp_packet &pkt, uint32_t flags, string data)
{
  memset(&pkt, 0, sizeof(pkt));
  pkt.seq_num = sequence_number;
  pkt.ack_num = ack_number;
  /*If it's an ACK packet, let data be empty*/
  pkt.flags &= 0x00;
  pkt.flags |= flags;
  pkt.len_data = data.length();
  
  switch(flags)
  {
    case DATA:
      sequence_number++;
      memcpy(pkt.data, data.c_str(), pkt.len_data);
      break;
  }
}

packet_meta TCP::make_meta(tcp_packet *pkt)
{
  packet_meta ret;
  memset(&ret, 0, sizeof(ret));
  ret.pkt = pkt;
  ret.was_acked = false;
  return ret;
}

void TCP::transmit_pkt(tcp_packet &pkt)
{
   switch(pkt.flags)
  {
    case RETRANS: 
      printf("Sending DATA packet %d Retransmission\n", pkt.seq_num);
      break;
    case SYN:
      printf("Sending SYN packet %d\n", pkt.seq_num);
      break;
    case DATA:
     printf("Sending DATA packet %d\n", pkt.seq_num);
      break;
    case FIN:
      printf("Sending FIN packet %d\n", pkt.seq_num);
      break;
    case SYNACK:
      printf("Sending ACK packet %d\n", pkt.seq_num);
      break;
    case ACK:
      printf("Sending ACK packet %d\n", pkt.ack_num);
    break;

  }
  addrlen = sizeof(remaddr);
	int bytes_sent;
	bytes_sent = sendto(fd, &pkt, sizeof(pkt), 0, 
    (struct sockaddr *)&remaddr, addrlen);
	if(bytes_sent <= 0)
		fatal_error("sendto");
 

  sequence_number++;
  
}

void TCP::recv_pkt(tcp_packet &pkt)
{ 
    memset(&pkt, 0, sizeof(pkt));
    addrlen = sizeof(remaddr);
    int recvlen = recvfrom(fd, &pkt, sizeof(pkt), 0, 
    (struct sockaddr *)&remaddr, &addrlen);
	
    if (recvlen >= 0) {
      switch(pkt.flags)
    {
      /*Every time you receive a data packet, send the ACK packet*/
      case DATA:
        printf("Receiving DATA packet %d\n", pkt.seq_num);

        ack_number = pkt.seq_num+1;
        
        tcp_packet ack;
        make_packet(ack, ACK, "");
        transmit_pkt(ack);
        break;
      /*If it's an ACK packet, set the sequence number
      to ack_num*/
      case ACK:
        printf("Receiving ACK packet %d\n", pkt.ack_num);
        break;
      case SYN:
        printf("Receiving SYN packet %d\n", pkt.seq_num);
        break;
      case SYNACK:
        printf("Receiving ACK packet %d\n", pkt.ack_num);
        break;
    }
            
    }
    else
      perror("recv_pkt error");

    
    
}

void TCP::resize(char * arr) {
  int size = sizeof(arr);
  size_t newSize = size * 2;
  char* newArr = new char[newSize];
  memcpy( newArr, arr, size * sizeof(char) );
  delete [] arr;
  arr = newArr;
}


void TCP::print_addr_info()
{
	char hostip[64];
	char remip[64];
	inet_ntop(AF_INET, &(hostaddr.sin_addr), hostip, 64);
	inet_ntop(AF_INET, &(remaddr.sin_addr), remip, 64);
	printf("Host port:%d\n", hostaddr.sin_port);
	printf("Host IP:%s\n", hostip);
	printf("Rem port:%d\n", remaddr.sin_port);
	printf("Rem IP:%s\n", remip);
	
}

void TCP::set_acks(uint32_t acknum)
{
  vector<packet_meta>::iterator it = packet_meta_data.begin();
  for(; it != packet_meta_data.end(); it++)
    it->was_acked = true;
}




