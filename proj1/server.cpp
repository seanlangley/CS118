#include <vector>
#include <fstream>
#include "Sock.h"
#include "server.h"
using namespace std;


void error(string msg)
{
  cout << "CALLED ERROR";
  const char *c_msg = msg.c_str();
  perror(c_msg);
  exit(1);
}


bool get_file(string &file, char *file_buf, string &file_name, int &length)
{
  bool found = true;
  char *buffer = NULL;
  length = 0;
  ifstream is (file_name, ifstream::binary);
  //If the file isn't found, return the 404 file
  if(!is)
    {
      ifstream ip ("not_found.html", ifstream::in);
      ip.seekg(0, ip.end);
      length = ip.tellg();
      ip.seekg(0, ip.beg);
      
      buffer = new char[length];

      ip.read(buffer, length);
      if(!ip)
	error("Error reading 404 File.");
      ip.close();
      found = false;
		
    }
  //Otherwise, return the requested file
  else if (is) {
    is.seekg (0, is.end);
    length = is.tellg();
    is.seekg (0, is.beg);

    buffer = new char [length];

    is.read (buffer,length);    
    if(!is)
      error("Error reading File.");
    is.close();

  }

  file_buf = new char[length];

  strcpy(file_buf, buffer);

  file = buffer;
  return found;

}


void parse_requests(string &file_name, string &file_ext, char *buffer)
{
  char *buffer_copy = new char[4096];
  char *buffer_copy2 = new char[4096];
  strcpy(buffer_copy, buffer);
  strcpy(buffer_copy2, buffer);
  
  char *name = strtok(buffer_copy, "/");  
  name = strtok(NULL, " ");
  file_name = name;
  
  char *type = strtok(buffer_copy2, "/");
  type = strtok(NULL, ".");
  type = strtok(NULL, " ");
  //Instead of html, add text/html
  //  requests->push_back(file_type);

  if(strcmp(type, "html") == 0)
    file_ext = "text/html";
  else if(strcmp(type, "jpg") == 0)
    file_ext = "image/jpg";
  else if(strcmp(type, "gif") == 0)
    file_ext = "image/gif";
  delete[] buffer_copy;
  delete[] buffer_copy2;

}

void getoptions(int &portno, int argc, char *argv[])
{
  int c;
  while((c = getopt(argc, argv, "p:")) != -1)
  {
   switch(c){
      case 'p':
        portno = atoi(optarg);
        break;
      case '?':
        if(optopt == 'p')
          fprintf(stderr,"Option -p requires a port number.\n");
        else if(isprint(optopt))
          exit(1);
        else
            fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
        break;
      default: abort();
    }

  }
}


int main(int argc, char *argv[])
{

  int c;
  int portno = 10101;  
  getoptions(portno, argc, argv);

    Socket mysock(portno);

    int n;
    char buffer[16000];

    memset(buffer, 0, 16000);  // reset memory
    
    //read client's message
    n = read(mysock.fd(), buffer, 16000);
    if (n < 0) error("ERROR reading from socket");
    printf("%s\n", buffer);
    
    //Only need file name and extension
    string file_name;
    string  file_ext;
   
    parse_requests(file_name, file_ext, buffer);


    string file;
    char *file_buf = NULL;
     int file_length;
    bool was_found = get_file(file, file_buf, file_name, file_length);    
/*TODO: Figure out how to get file_buf as a copy of the input file, without previously
knowing the length*/

    

    //reply to client  
    vector<string> responses;
    if(was_found) responses.push_back("HTTP/1.1 200 OK\r\n");    
    else responses.push_back("HTTP/1.1 404 Not Found\r\n");

    responses.push_back("Content-Type: ");
    responses.push_back(file_ext);
    responses.push_back("\r\n");
    responses.push_back("Content-Length: ");
    responses.push_back(to_string(file.size()));
    responses.push_back("\r\n\r\n");

    
   for(unsigned long i = 0; i < responses.size(); i++)
   {
    	n = send(mysock.fd(), responses[i].c_str(), responses[i].size(), 0);	
	   if(n < 0) error("Error writing to socket");
   }

    
    
       
    if(file_ext == "text/html")
    {
      n = send(mysock.fd(), file.c_str(), file.size(), 0);
      if(n < 0) error("ERROR writing to socket");    
    }
   

   else if(file_ext == "image/jpg")
   {
    
      n = send(mysock.fd(), file_buf, file_length, 0);
   }

    return 0;

}












