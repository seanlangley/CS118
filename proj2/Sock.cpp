#include "Sock.h"
using namespace std;
void Socket::error(string msg)
{
	cout << "CALLED ERROR" << endl;
	
  	const char *c_msg = msg.c_str();
  	perror(c_msg);
  	exit(1);
}
//Constructor for server socket
Socket::Socket(int port_no)
{
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
        error("ERROR opening socket");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));  // reset memory
    memset((char*) &cli_addr, 0, sizeof(cli_addr));

    // fill in address info
    portno = port_no;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    //listen(sockfd, 5);  // 5 simultaneous connection at most

    //accept connections
    //newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    //if (newsockfd < 0)
   //  error("ERROR on accept");
  
}

Socket::Socket(int portno, int flag)
{

  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      error("ERROR opening socket");
   server = gethostbyname("localhost");

  if(server == NULL)
    {
      fprintf(stderr, "Error creating host\n");
      exit(1);
    }
  memset(&serv_addr, '0', sizeof(serv_addr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;
  
  

 
   if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
       error("\n Error : Connect Failed \n");
     


}

Socket::~Socket()
{
	close(newsockfd);
	close(sockfd);
 	
}
