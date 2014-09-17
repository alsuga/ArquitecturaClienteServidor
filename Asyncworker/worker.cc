#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char *itoa(int in){
  char out[20];
  sprintf(out,"%i",in);
  return out;
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  char out[20];
  int a,b;
  zmsg_append(out_msg, &wh);
  if(type == '+'){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg)) + a;
    zmsg_addstr(out_msg,itoa(b,out));
  }
  if(type == '-'){
    a = atoi(zmsg_popstr(in_msg));
    b = a - atoi(zmsg_popstr(in_msg));
    zmsg_addstr(out_msg,itoa(b,out));
  }
  if(type == '*'){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg)) * a;
    zmsg_addstr(out_msg,itoa(b,out));
  }
  if(type == '/'){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg));
    if(b == 0){ 
      zmsg_addstr(out_msg,"division por cero");
      return;
    }
    zmsg_addstr(out_msg,itoa(a/b,out));
  }
  zframe_destroy(&wh);
}


int main(int argc, char** argv) {
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);
  zsocket_bind(server, "tcp://localhost:5555");
  
  zmsg_t *reportmsg;
  zmsg_addstr(reportmsg,argv[1]);
  zmsg_send(&reportmsg,server);
 
  while(true) {
    zmsg_t *outmsg,*inmsg = zmsg_recv(server);
    zmsg_print(inmsg);
    dispatcher(inmsg,outmsg);
    zmsg_send(&outmsg,server);  
  }
  zctx_destroy(&context);
  return 0;
}
