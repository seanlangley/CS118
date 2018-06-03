#include <iostream>
#include <cstdlib>
#include <cstring> // memset, etc.
#include <signal.h>
#include <netdb.h> // hostent, etc.
#include <arpa/inet.h> // inet_htop
#include <fstream>
#include "RDTConnection.h"

using namespace std;
#define DEFAULT_PORT 65280
#define WINDOW_SIZE 1024

RDTConnection *conn = NULL;

void sig_handler( int signal ) {
    cout << "Caught signal " << signal << ", exiting" << endl;

    if(conn) {
        delete conn;
        conn = NULL;
    }

    exit(signal);
}

int main( int argc, char** argv ) {
    signal( SIGHUP, sig_handler );
    signal( SIGINT, sig_handler );
    signal( SIGTERM, sig_handler );

    string hostname = "localhost";
    int port = DEFAULT_PORT;
    string file_name = "index.html";
    double pdrop = 0;
    double pcorrupt = 0;

    switch (min(argc, 5)) {
        case 5:
            pcorrupt = atof(argv[--argc]) * 100;
        case 4:
            pdrop = atof(argv[--argc]) * 100;
        case 3:
            file_name = argv[--argc];
        case 2:
            port = atoi(argv[--argc]);
        case 1:
            hostname = argv[--argc];
        case 0: // program name
        default:
            break;
    }
    hostname = "localhost";
    struct hostent *hp = gethostbyname(hostname.c_str());
    char ip_addr[INET_ADDRSTRLEN];
    memset(&ip_addr, 0, sizeof(ip_addr));

    if (hp && hp->h_length > 0) {
        inet_ntop(AF_INET, hp->h_addr_list[0], ip_addr, sizeof(ip_addr));
    } else {
        cout << "Failed to look up host named: " << hostname << ", aborting" << endl;
        exit(-1);
    }

    conn = new RDTConnection(WINDOW_SIZE, pdrop, pcorrupt);

    if (!conn->connect(ip_addr, port)) {
        cout << "Connection failed, aborting" << endl;
        exit(-1);
    }

    string server_data;
    conn->send_data(file_name);
    conn->receive_data(server_data);
    conn->close();
    ofstream outfile;
    outfile.open("output.jpg", ios::binary | ios::out);
    outfile.write(server_data.c_str(), server_data.size());
   // cout << server_data;
    return 0;
}