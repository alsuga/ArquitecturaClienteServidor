#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char *itoa(int in,char *out){
  sprintf(out, "%i\0", in);
  return out;
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  char *tmp = zmsg_popstr(in_msg), out[20];
  int a,b;
  assert(tmp);
  zmsg_append(out_msg, &wh);
  if(strcmp(tmp,"+") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg)) + a;
    zmsg_addstr(out_msg,itoa(b,out));
  }
  if(strcmp(tmp,"-") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = a - atoi(zmsg_popstr(in_msg));
    zmsg_addstr(out_msg,itoa(b,out));
  }
  if(strcmp(tmp,"*") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg)) * a;
    zmsg_addstr(out_msg,itoa(b,out));
  }
  if(strcmp(tmp,"/") == 0){
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
  void* server = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(server, "tcp://*:5555");
  zsocket_bind(client, "tcp://localhost:5555");

  zmsg_t *reportmsg;
  zframe_t *identity = zframe_new(zsocket_identity(client));
  zmsg_prepend(reportmsg,&identity);
  zmsg_addstr(reportmsg,argv[1]);
  zmsg_send(&reportmsg,client);
  zmsg_destroy(&reportmsg);
  zframe_destroy(&identity);
  zsocket_destroy(&client);


  int i = 0;
  while(true) {
    zmsg_t *outmsg,*inmsg = zmsg_recv(server);
    // Print the massage on the server's console
    zmsg_print(inmsg);
    
    dispatcher(inmsg,outmsg);
    zmsg_send(&outmsg,server);
    zmsg_destroy(&inmsg);
    zmsg_destroy(&outmsg);
  }
  zctx_destroy(&context);
  return 0;
}
