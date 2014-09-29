#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

int main(int argc, char **argv) {
  int leader = atoi(argv[1]);
  char *act = argv[1];
  zctx_t* context = zctx_new();
  void* recive= zsocket_new(context,ZMQ_ROUTER);
  string port = "tcp://localhost:";
  port += argv[2];
  zsocket_bind(recive, port.c_str);
  void* send = zsocket_new(context,ZMQ_DEALER);
  port = "tcp://localhost:";
  port += argv[3];
  zsocket_bind(send, port.c_str);

  zmq_pollitem_t items[] = {{recive,0,ZMQ_POLLIN,0}};

  cout<<"Estamos listos!"<<endl;
  int soy = atoi(argv[4]);
  if(soy){
    zmsg_t *outmsg = zmsg_new();
    outmsg.add_str(argv[1]);
    zmsg_send(&outmsg,send);
  }

  while(true) {
    zmq_poll(items, 2,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN){
      cout<<"Mensaje de worker recibido"<<endl;
      zmsg_t *inmsg = zmsg_recv(recive);
      char *tmp = inmsg.popstr(inmsg);
      zmsg_t *outmsg = zmsg_new();
      int in = atoi(tmp);
      if(leader < in){
        leader = in;
        strcp(tmp,act);
        outmsg.push_str(tmp);
        zmsg_send(&outmsg,send);
      }else{
        outmsg.push_str(act);
        zmsg_send(&outmsg,send);
        if(leader == in) break;
      }
    }
  }
  zctx_destroy(&context);
  return 0;
}
