#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

unordered_map<string, vector<zframe_t*> > wr;

/******* WORKERS ***********/

void handleWorker(zmsg_t *msg, void *clients){
  zmsg_print(msg);
  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  if(strcmp(op,"register") == 0){
    char *operation = zmsg_popstr(msg);
    wr[operation].push_back(zframe_dup(id));
  }
  if(strcmp(op,"answer") == 0){
    zmsg_send(&msg,clients);
  }
  free(op);
  zframe_destroy(&id);
  zmsg_destroy(&msg);
}


/******* Clientes *********/

void handleClient(zmsg_t *msg, void *workers){
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  //IMPORTANTE
  zframe_t* worker = getWorke
}

int main(void) {
  zctx_t* context = zctx_new();
  void* clients = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(clients, "tcp://*:5555");
  void* workers = zsocket_new(context,ZMQ_DEALER);
  zsocket_bind(workers, "tcp://*:4444");

  zmq_pollitem_t items[] = {{workers,0,ZMQ_POLLIN,0},
                            {clients,0,ZMQ_POLLIN,0}};

  cout<<"Estamos listos!"<<endl;

  while(true) {
    zmq_poll(items, 2,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN){
      zmsg_t *inmsg = zmsg_recv(workers);
      handleWorker(msg, clients);
    }
    if(items[1].revents & ZMQ_POLLIN){
      zmsg_t *inmsg = zmsg_recv(clients);
      handleClient(msg,workers);
    }
  }
  zctx_destroy(&context);
  return 0;
}
