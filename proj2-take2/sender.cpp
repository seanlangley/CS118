#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include "rdt.h"
#include "helpers.h"
using namespace std;


int main(int argc, char **argv)
{
	rdt sender;
	sender.send_init();
	string file_name = "file.txt";
	string file;
	getfile(file, file_name);
	uint32_t offset = 0;
	
	string data = "";
	/*One packet case*/
	if(file.size() < MSS)
	{
		data = sender.rdt_send(file);
		while(data == file)
			data = sender.rdt_send(file);

	}
	
	else
		while(offset+MSS != file.size())
		{
			data  = file.substr(offset, MSS);
			offset += MSS;
			string confirm = data;
			while(confirm == data)
				confirm = sender.rdt_send(data);
		}
	



	
}
