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

// pthread_mutex_t start_mutex;
// pthread_mutex_t stop_mutex;

// void* timeout(void*)
// {
// 	return NULL;
// }

int main(int argc, char **argv)
{
	rdt sender;

	// pthread_t for_timeout;
	// int rc = pthread_create(&for_timeout, NULL, timeout, NULL);

	// Timeout Initialization
	// pthread_mutex_lock(&start_mutex);
	sender.set_start(clock());

	// pthread_mutex_lock(&stop_mutex);
	sender.set_stop(sender.get_start());
	// pthread_mutex_unlock(&start_mutex);
	// pthread_mutex_unlock(&stop_mutex);

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
