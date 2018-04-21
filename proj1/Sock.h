#ifndef Sock
#define Sock

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <ctype.h>
using namespace std;

class Socket{
 public:
  Socket(int port_no);
  ~Socket();
  void error(string msg);
  inline int fd(){return newsockfd;}
 private:
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

};

#endif
