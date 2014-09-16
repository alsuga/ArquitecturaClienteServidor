#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

int main(int argc, char** argv) {

  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);
  //cout << "client identity: " << zsocket_identity(client) << endl;

  zsocket_connect(server, "tcp://localhost:5555");
  zmsg_t *msg = zmsg_new();
  for(int i = 0; i < 3; i++) zmsg_addstr(argv[i+1]);
  zmsg_send(client, msg);

  cout<<"Ya le llega relajese"<<endl;

  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      cout << "Ya llego!!:\n";
      zmsg_t *incmsg = zmsg_recv(server);
      zmsg_print(incmsg);
      zmsg_destroy(&incmsg);
      zmsg_destroy(&msg);
      break;
    }
  }
  zctx_destroy(&context);
  return 0;
}
