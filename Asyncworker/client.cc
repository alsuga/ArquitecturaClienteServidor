#include <bits/stdc++.h>
#include <czmq.h>

#define endl '\n'

using namespace std;

int main(int argc, char** argv) {

  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);

  zsocket_connect(server, "tcp://localhost:5555");
  zmsg_t *msg = zmsg_new();
  for(int i = 0; i < 3; i++) zmsg_addstr(argv[i+1]);
  zmsg_send(client, msg);

  cout<<"Ya le llega relajese"<<endl;

  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      cout << "Ya llego!!:"<<endl;
      zmsg_t *incmsg = zmsg_recv(server);
      zmsg_print(incmsg);
      zmsg_destroy(&incmsg);
      zmsg_destroy(&msg);
      break;
    }
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
