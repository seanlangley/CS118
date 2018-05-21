#ifndef helpers
#define helpers
#include <fstream> 
using namespace std;
void error(string msg)
{
	cout << "CALLED ERROR" << endl;
	
  	const char *c_msg = msg.c_str();
  	perror(c_msg);
  	exit(1);
}

void getfile(string &file, string &file_name)
{
  char *buffer = NULL;
  file = "";
  
  ifstream is;
  is.open(file_name.c_str(), ifstream::in);
  //If the file isn't found, return the 404 file
  if(!is)
       error("Reqeusted file not found.");
  //Otherwise, return the requested file
  else if (is) {
    char c = is.get();
    while(is.good())
    {
      file.push_back(c);
      c = is.get();

    }
  }

  is.close();

}

#endif