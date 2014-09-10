#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

int main(int argc, char** argv) {

  zctx_t* context = zctx_new();
  void* client = zsocket_new(context,ZMQ_DEALER);

  if (argc > 1) {
    char *identity = argv[1];
    zsocket_set_identity(client,identity);
  }
  
  zsocket_connect(client, "tcp://localhost:5555");

  zmq_pollitem_t item = {client, 0, ZMQ_POLLIN, 0};
  
  string in;

  while(true) {
    zmq_poll(&item,1,10*ZMQ_POLL_MSEC);
    if(item.revents & ZMQ_POLLIN) {
      zmsg_t *msg = zmsg_recv(client);
      zmsg_print(msg);
      zmsg_destroy(&msg);      
    }
    cin>>in;
    zstr_send(client, in.c_str);
  }
  zctx_destroy(&context);
  return 0;
}
