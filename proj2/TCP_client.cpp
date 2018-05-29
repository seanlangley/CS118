
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
	transmit_pkt(pkt);
	/*Receive the ACK, set the ACKNUM*/
	recv_pkt(pkt);
    /*Send the SYN-ACK*/
    /*ACKs don't increase the seq_num*/
    make_packet(pkt, ACK, "");
	transmit_pkt(pkt);

}

string TCP_client::request_file(string file_name)
{
	cout << "Requesting file " << file_name << endl;
	tcp_packet pkt;

	make_packet(pkt, DATA, file_name);
	transmit_pkt(pkt);
	/*Wait for ACK*/
	recv_pkt(pkt);
	/*Do something on ACK, not sure yet*/

	/*Receive file*/
	uint8_t fin_flag = 0x11;
	string ret = "";
	ofstream outfile;
  	outfile.open("output.txt", ios::binary | ios::out);
  	// outfile.open("output.jpg", ios::binary | ios::out);
	recv_pkt(pkt);
	// printf("flags: %d\n", pkt.flags);

	while ((fin_flag & pkt.flags) != 0x11)
	{
		outfile.write(pkt.data, pkt.len_data);		//pkt.len_data			sizeof(pkt.data)
		// outfile << pkt.data;
		// for (int i = 0; i < pkt.len_data; i++)
		// {
		// 	// if (pkt.data[i] == EOF)
		// 	// {
		// 	// 	printf("EOF\n");
		// 	// 	break;
		// 	// }
		// 	outfile << pkt.data[i];
		// }
		make_packet(pkt, ACK, "");
		transmit_pkt(pkt);
		recv_pkt(pkt);
	}

	outfile.close();
	// recv_pkt(pkt);
	// string ret = pkt.data;
	/*Send the ACK*/
	make_packet(pkt, ACK, "");
	transmit_pkt(pkt);
	return ret;




	// /*Receive file*/
	// uint8_t fin_flag = 0x11;
	// string ret = "";
	// std::ofstream out("output.jpg");
	// recv_pkt(pkt);
	// // printf("flags: %d\n", pkt.flags);

	// while ((fin_flag & pkt.flags) != 0x11)
	// {
	// 	out << pkt.data;
	// 	make_packet(pkt, ACK, "");
	// 	transmit_pkt(pkt);
	// 	recv_pkt(pkt);
	// }

	// out.close();
	// // recv_pkt(pkt);
	// // string ret = pkt.data;
	// /*Send the ACK*/
	// make_packet(pkt, ACK, "");
	// transmit_pkt(pkt);
	// return ret;

}


void TCP_client::teardown()
{
	// /*Send FIN*/
	// tcp_packet pkt;
	// make_packet(pkt, FIN, "");
	// transmit_pkt(pkt);
	// /*Wait for ACK*/
	// recv_pkt(pkt);
	// /*Wait for FIN*/
	// recv_pkt(pkt);
	// /*Send ACK*/
	// make_packet(pkt, ACK, "");
	// transmit_pkt(pkt);
	// close(fd);



		/*Wait for FIN*/
	tcp_packet pkt;
	recv_pkt(pkt);
	/*Send ACK*/
	make_packet(pkt, ACK, "");
	transmit_pkt(pkt);
	/*Send FIN*/
	make_packet(pkt, FIN, "");
	transmit_pkt(pkt);
	/*Wait for ACK*/
	recv_pkt(pkt);
	close(fd);
}











