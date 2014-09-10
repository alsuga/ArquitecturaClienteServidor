#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char type;

char *itoa(int in,char *out){
  sprintf(out, "%i\0", in);
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
  void* client = zsocket_new(context,ZMQ_DEALER);
  zsocket_bind(client, "tcp://localhost:5555");
  type = argv[1][0];
  zmsg_t *reportmsg;
  zmsg_addstr(reportmsg,argv[1]);
  zmsg_send(&reportmsg,client);
  zmsg_destroy(&reportmsg);
  while(true) {
    zmsg_t *outmsg,*inmsg = zmsg_recv(client);
    zmsg_print(inmsg);
    
    dispatcher(inmsg,outmsg);
    zmsg_send(&outmsg,client);
    zmsg_destroy(&inmsg);
    zmsg_destroy(&outmsg);
  }
  zctx_destroy(&context);
  return 0;
}
