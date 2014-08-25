//  Hello World client
#include <zmq.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int main (void){
    printf ("Connecting to my little server...\n");
    printf ("Send in format \"num op num\"\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    string tmp;
    int request_nbr;
    while(true) {
        char buffer [20];
        getline(cin,tmp);
        if(tmp == "exit") break;
        printf ("Sending operation \"%s\" \n", tmp.c_str());
        zmq_send (requester, tmp.c_str(), 20, 0);
        zmq_recv (requester, buffer, 20, 0);
        printf ("Received answer %s\n", buffer);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
