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
  
  cout << "client identity: " << zsocket_identity(client) << endl;

  zsocket_connect(client, "tcp://localhost:5555");

  zmq_pollitem_t items[] = {{client, 0, ZMQ_POLLIN, 0}};
  int request_nbr = 0;

  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      // This is executed if there is data in the client socket that corresponds
      // to items[0]
      cout << "Incomming message:\n";
      zmsg_t *msg = zmsg_recv(client);
      zmsg_print(msg);
      // zframe_print(zmsg_last(msg), identity);
      zmsg_destroy(&msg);      
    }

    zstr_sendf(client, "request #%d", request_nbr);
    request_nbr++;
  }
  zctx_destroy(&context);
  return 0;
}
