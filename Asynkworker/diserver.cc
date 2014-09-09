#include <iostream>
#include <set>
#include <string>
#include <czmq.h>
#include <string.h>

using namespace std;

int main(void) {
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(server, "tcp://*:5555");
  void* client = zsocket_new(context,ZMQ_DEALER);

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
