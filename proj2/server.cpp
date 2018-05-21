// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include "helpers.h"
using namespace std;
#define PORT     8080
#define MAXLINE 1024
 
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    string hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;
     
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
     
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = 10000;
     
    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
     
    int n;
    socklen_t len = 0;;
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                &len);
    if(n < 0) {cout << n << " bytes sent. "; error("recvfrom");}
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    cout << "Client IP is " << inet_ntoa(cliaddr.sin_addr) << endl;
    cout << "Client port is " << cliaddr.sin_port << endl;
    cout << "Server IP is " << inet_ntoa(servaddr.sin_addr) << endl;
    cout << "Sever port is " << servaddr.sin_port << endl;

    n = sendto(sockfd, hello.c_str(), hello.size(), 
        0, (const struct sockaddr *) &cliaddr,
            len);
    if(n < 0){cout << n << " bytes sent. "; error("Sendto");}

    
    printf("Hello message sent.\n"); 
     
    return 0;
}
