#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char *itoa(int in){
  char out[20];
  sprintf(out,"%i",in);
//  tmp= itos(in);
//  strcpy(out,tmp.c_str);
  return out;
}

void dispacher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  char *tmp = zmsg_popstr(in_msg);
  int a,b;
  assert(tmp);
  zmsg_append(out_msg, &wh);
  if(strcmp(tmp,"+") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg)) + a;
    zmsg_addstr(out_msg,itoa(b));
  }
  if(strcmp(tmp,"-") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = a - atoi(zmsg_popstr(in_msg));
    zmsg_addstr(out_msg,itoa(b));
  }
  if(strcmp(tmp,"*") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg)) * a;
    zmsg_addstr(out_msg,itoa(b));
  }
  if(strcmp(tmp,"/") == 0){
    a = atoi(zmsg_popstr(in_msg));
    b = atoi(zmsg_popstr(in_msg));
    if(b == 0){ 
      zmsg_addstr(out_msg,"division por cero");
      return;
    }
    zmsg_addstr(out_msg,itoa(a/b));
  }
  zframe_destroy(&wh);
}

int main(void) {
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);
  zsocket_bind(server, "tcp://localhost:4444");

  zmsg_t *outmsg;
  zframe_t *identity = zsocket_identity(server); 
  
  int i = 0;
  while(true) {
    zmsg_t *outmsg,*inmsg = zmsg_recv(server);
    // Print the massage on the server's console
    zmsg_print(msg);
    
    dispatcher(inmsg,outmsg);
    zmsg_send(outsmg,sever);
    zmsg_destroy(&inmsg);
    zmsg_destroy(&outmsg)
  }
  zctx_destroy(&context);
  return 0;
}
