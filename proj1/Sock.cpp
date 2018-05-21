#include "Sock.h"
using namespace std;
void Socket::error(string msg)
{
	cout << "CALLED ERROR" << endl;
	
  	const char *c_msg = msg.c_str();
  	perror(c_msg);
  	exit(1);
}

Socket::Socket(int port_no)
{
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
        error("ERROR opening socket");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));  // reset memory

    // fill in address info
    portno = port_no;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);  // 5 simultaneous connection at most

    //accept connections
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0)
     error("ERROR on accept");
  
}

Socket::~Socket()
{
	close(newsockfd);
	close(sockfd);
 	
}