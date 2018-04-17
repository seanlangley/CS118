/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <signal.h>  /* signal name macros, and the kill() prototype */
#include <vector>
#include <fstream>
using namespace std;


void error(string msg)
{
  const char *c_msg = msg.c_str();
  perror(c_msg);
  exit(1);
}

string get_time()
{

  time_t result = time(NULL);
  char t[50];
  memset(t,'\0', sizeof(t));
  strcpy(t, asctime(gmtime(&result)));
  string time = t;
  time.pop_back();

  return time;
}

void get_file(char *send)
{
  char *buffer = NULL;
  int length = 0;
  ifstream is ("test.html", ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    length = is.tellg();
    is.seekg (0, is.beg);

    buffer = new char [length];



    // read data as a block:
    is.read (buffer,length);
    

    if(!is)
      error("Error reading File.");
    is.close();

    // ...buffer contains the entire file...
    /*Testing
    for(int i = 0; i < length; i++)
      {
	printf("%c", buffer[i]);
      }
    */

  }

  if(buffer != NULL)
    strcpy(send, buffer);
  else
    send[0] = '\0';

  return;

}


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // create socket
    if (sockfd < 0)
        error("ERROR opening socket");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));  // reset memory

    // fill in address info
    portno = atoi(argv[1]);
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

    int n;
    char buffer[16000];

    memset(buffer, 0, 16000);  // reset memory

    //read client's message
    n = read(newsockfd, buffer, 16000);
    if (n < 0) error("ERROR reading from socket");
    printf("%s\n", buffer);


    //reply to client    
    vector<string> responses;
    responses.push_back("HTTP/1.1 200 OK\r\n");
    responses.push_back("Connection: close\r\n");        
    responses.push_back(get_time());
    responses.push_back("\r\n");
    responses.push_back("Content-Length: 5102\r\n");
    responses.push_back("Content-Type: text/html\r\n");
    responses.push_back("\r\n");    
        
    
    for(unsigned long i = 0; i < responses.size(); i++)
      {
	cout << responses[i];
	n = write(newsockfd, &responses[i], sizeof(responses[i]));
	if (n < 0) error("ERROR writing to socket");
      }
    
    char file[5102];
    get_file(file);
    

    if(file[0] == '\0')
      error("Error getting file.");


    for(int i = 0; file[i] != '\0'; i++)
      printf("%c", file[i]);
    n = write(newsockfd, &file, sizeof(file));
    if(n < 0) error("ERROR writing to socket");
    

    
    close(newsockfd);  // close connection
    close(sockfd);

    return 0;

}
