
#include <fstream> 
#include <iostream>
#include <stdlib.h>
using namespace std;




inline void fatal_error(string msg)
{
	cout << "FATAL ERROR" << endl;
	
  	const char *c_msg = msg.c_str();
  	perror(c_msg);
  	exit(1);
}

inline void getfile(string &file, string &file_name)
{

  file = "";
  
  ifstream is;
  is.open(file_name.c_str(), ios::in | ios::binary);
  if(!is)
       fatal_error("Requested file not found.");
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
