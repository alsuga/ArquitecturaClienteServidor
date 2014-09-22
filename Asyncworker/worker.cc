#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

int type;

char *itoa(int in){
  char *out= new char;
  sprintf(out,"%i",in);
  return out;
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  int a = atoi(zmsg_popstr(in_msg)),b = atoi(zmsg_popstr(in_msg));
  zmsg_addstr(out_msg,"answer");
  zmsg_append(out_msg, &wh);
  if(type == '+') zmsg_addstr(out_msg,itoa(a+b));
  if(type == '-') zmsg_addstr(out_msg,itoa(a-b));
  if(type == '*') zmsg_addstr(out_msg,itoa(a*b));
  if(type == '/'){
    if(b == 0) zmsg_addstr(out_msg,"division por cero");
    else zmsg_addstr(out_msg,itoa(a/b));
  }
  zframe_destroy(&wh);
}


int main(int argc, char** argv) {
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);
  zsocket_bind(server, "tcp://localhost:444");
  zmsg_t *reportmsg= zmsg_new();
  zmsg_addstr(reportmsg,"reporting");
  zmsg_addstr(reportmsg,argv[1]);
  sscanf(argv[1],"%i",&type);
  zmsg_send(&reportmsg,server);

  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};

  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      cout << "Tengo trabajo!!"<<endl;
      zmsg_t *incmsg = zmsg_recv(server);
      zmsg_t *outmsg = zmsg_new();
      zmsg_print(incmsg);
      zframe_t *cdic = zmsg_pop(incmsg);
      dispatcher(incmsg,outmsg);
      zmsg_destroy(&incmsg);
      zmsg_send(&outmsg,server);
    }
  }
  zctx_destroy(&context);
  return 0;
}
