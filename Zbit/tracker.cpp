#include<bits/stdc++.h>
#include<czmq.h>

#define SIZEOFPART 524288
#define endl '\n'
#define _dbg(x) if(1) cout<<"----------------------"<<x<<"------------------------"<<endl;


using namespace std;

// Crear estructura de datos para guardar quien tiene las partes
map<string,map<int,vector<string> > > info;


int main(int argc, const char *argv[]){
  string dir = "tcp://*:";
  if(argc > 2) dir += argv[1];
  else dir += "5555";
  zctx_t *context = zctx_new();
  void *clients = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(clients,dir.c_str());


  zmq_pollitem_t items[] = {{clients,0,ZMQ_POLLIN,0}};

  cout<<"Estamos listos!"<<endl;

  while(true ) {
    zmq_poll(items, 2,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN){
      cout<<"Mensaje de worker recibido"<<endl;
      zmsg_t *inmsg = zmsg_recv(clients);
      //thread t(handleWorker,inmsg,clients,workers);
      //handleClient(inmsg, clients);
      cout<<"Mensaje de worker despachado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}



void handler(zmsg_t *msg){
  zframe_t *dir = zmsg_pop(msg);
  string op = zmsg_popstr(msg);
  if(op == "report"){
    string client = zmsg_popstr(msg),song;
    while(zmsg_size(msg) > 0){
      song = zmsg_popstr(msg);
    }
  }
  if(op == "request"){
  }
  if(op == "npart"){
  }
}
