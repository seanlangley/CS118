#include <vector>
#include <fstream>
#include "Sock.h"
using namespace std;


void error(string msg)
{
  cout << "CALLED ERROR";
  const char *c_msg = msg.c_str();
  perror(c_msg);
  exit(1);
}


bool get_file(string &file, string &file_name, int &length)
{
  bool found = false;
  char *buffer = NULL;
  file = "";
  length = 0;
  
  //Parse through the file name. If it has a %20, that means
  //There's a space. Get rid of the %20 and replace it with a space.
  for(int i = 0; i < file_name.size(); i++)
  {
    if(file_name[i] == '%' && file_name[i+1] == '2' && file_name[i+2] == '0')
        {
          file_name.erase(i, 3);
          file_name.insert(i, " ");
        }

  }

/****TODO Figure out how to open files such that it's case insensitive. 
Test case e) from Zengwen's email.
****/


  ifstream is;
  is.open(file_name.c_str(), ifstream::in);
  //If the file isn't found, return the 404 file
  if(!is)
    {
          is.open("not_found.html", ifstream::in);
          char c = is.get();
          while(is.good())
        {
          file.push_back(c);
          c = is.get();

        }

    }
  //Otherwise, return the requested file
  else if (is) {
    char c = is.get();
    while(is.good())
    {
      file.push_back(c);
      c = is.get();

    }
    found = true;
  }

  is.close();
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

  if(strcmp(type, "html") == 0 || strcmp(type, "htm") == 0)
    file_ext = "text/html";
  else if(strcmp(type, "jpg") == 0 || strcmp(type, "jpeg") == 0)
    file_ext = "image/jpg";
  else if(strcmp(type, "gif") == 0)
    file_ext = "image/gif";
  else
    file_ext = "text/plain";
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
  int portno;
  //First command line argument contains the port number, 
  //default port is 10101
  if(argc == 2)
    portno = atoi(argv[1]);
  else
    portno = 10101;  
  //Add options here later potentially
  //getoptions(portno, argc, argv);

    Socket mysock(portno);

    int n;
    char buffer[16000];

    memset(buffer, 0, 16000);  // reset memory
    
    //read client's message
    n = read(mysock.fd(), buffer, 16000);
    if (n < 0) error("ERROR reading from socket");
    cout << "\n*** Client Request ***\n\n";
    printf("%s\n", buffer);
    
    //Only need file name and extension
    string file_name;
    string  file_ext;
   
    parse_requests(file_name, file_ext, buffer);


    string file;
     int file_length;
    bool was_found = get_file(file, file_name, file_length);    
   
     
    //reply to client  
    vector<string> responses;
    if(was_found) responses.push_back("HTTP/1.1 200 OK\r\n");    
    else 
      {
        responses.push_back("HTTP/1.1 404 Not Found\r\n");
        file_ext = "text/html";
      }

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

    n = send(mysock.fd(), file.c_str(), file.size(), 0);

    return 0;

}












