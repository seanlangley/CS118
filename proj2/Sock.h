#ifndef Sock
#define Sock

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ctype.h>
using namespace std;

class Socket{
 public:
  Socket(int port_no);
  Socket(int portno, int flag);
  ~Socket();
  void error(string msg);
  inline int fd(){return newsockfd;}
  inline int clifd(){return sockfd;}
  inline void cliaddr(struct sockaddr_in *cliaddr)
    {
      cliaddr = cli_addr;
    }
  inline *sockaddr_in servaddr(struct sockaddr_in *servaddr)
  {
    servaddr = serv_addr;
  }
public:
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  struct hostent *server;

};

#endif
