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
  // unsigned long header = sizeof(pkt.seq_num) + sizeof(pkt.ack_num) + sizeof(pkt.flags);
  // printf("size of header: %lu\n", header); //header is 9B
  
  switch(flags)
  {
    case DATA:
      // memcpy (pkt.data, data.c_str(), strlen(data.c_str())+1 );     //CHANGED HERE
      // strcpy(pkt.data, data.c_str());
      for (int i = 0; i < strlen(data.c_str()); i++)
      {
        pkt.data[i] = data[i];
      }
    break;
  }
}

void TCP::transmit_pkt(tcp_packet &pkt)
{
   switch(pkt.flags)
  {
    case SYN:
      printf("Sending packet %d SYN\n", pkt.seq_num);
      break;
    case DATA:
   printf("Sending packet %d\n", pkt.seq_num);
      break;
    case FIN:
      printf("Sending packet %d FIN\n", pkt.seq_num);
      break;
    case SYNACK:
      printf("Sending packet %d SYN ACK\n", pkt.seq_num);
      break;
    case ACK:
      printf("Sending packet %d\n", pkt.ack_num);
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
	printf("Receiving data\n");
    int recvlen = recvfrom(fd, &pkt, sizeof(pkt), 0, 
    (struct sockaddr *)&remaddr, &addrlen);
	
    if (recvlen >= 0) {
            printf("Receiving packet %d\n", pkt.ack_num);
            ack_number = pkt.seq_num + 1;
    }
    else
      perror("recv_pkt error");

    switch(pkt.flags)
    {
      /*If it's a data packet, increase the ack number to 
      seq_num+1*/
      case DATA:
        break;
      /*If it's an ACK packet, set the sequence number
      to ack_num*/
      case ACK:
        break;
    }
    
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






