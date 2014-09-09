#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char *itoa(int in){
  char tmp[10];
  while(in/=10){
    strcat(tmp,(char)(in%10 + '0'));
  }
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
  if(strcmp(tmp,"-") == 0)
  if(strcmp(tmp,"*") == 0)
  if(strcmp(tmp,"/") == 0)

}

int main(void) {
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(server, "tcp://*:5555");

  int i = 0;
  while(true) {
    zmsg_t *msg = zmsg_recv(server);
    // Print the massage on the server's console
    zmsg_print(msg);

    zframe_t *identity = zmsg_pop(msg);
    zframe_t *content = zmsg_pop(msg);
    assert(content);
    zmsg_destroy(&msg);

    zclock_sleep(2000);

    if (i % 5 == 0) {
      cout << "I'll answer this one!\n";
      zframe_send(&identity, server, ZFRAME_REUSE + ZFRAME_MORE);
      zframe_send(&content, server, ZFRAME_REUSE);
    } else {
      cout << "Skipping request\n";
    }
    i++;

    zframe_destroy(&identity);
    zframe_destroy(&content);
  }
  zctx_destroy(&context);
  return 0;
}
