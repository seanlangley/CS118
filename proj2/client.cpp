// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <iostream>
#include "helpers.h"
using namespace std;
#define PORT     10000
#define MAXLINE 1024
 
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    string hello = "Hello from client";
    struct sockaddr_in     servaddr;

 
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
 	
    memset(&servaddr, 0, sizeof(servaddr));


    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = PORT;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int n;
    socklen_t len;
     
    n = sendto(sockfd, hello.c_str(), hello.size(),
        0, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    if(n < 0){cout << n << " bytes sent. "; error("sendto");}
    

    printf("Hello message sent.\n");
   // cout << "Client IP is " << inet_ntoa(cliaddr.sin_addr) << endl;
   // cout << "Client port is " << cliaddr.sin_port << endl;
    cout << "Server IP is " << inet_ntoa(servaddr.sin_addr) << endl;
    cout << "Sever port is " << servaddr.sin_port << endl;
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    if(n < 0){cout << n << " bytes sent. "; error("recvfrom");}

    
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
 
    close(sockfd);
    return 0;
}