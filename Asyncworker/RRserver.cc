#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

map<string, vector<zframe_t*> > wr;
map<string, int> st;

zframe_t * getWorker(string operation){
  zframe_t *id = wr[operation][st[operation]];
  st[operation]= (st[operation]+1) % wr[operation].size();
  return zframe_dup(id);
}


/******* WORKERS ***********/

void handleWorker(zmsg_t *msg, void *clients){
  cout<<"Enviando a los clientes"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  if(strcmp(op,"reporting") == 0){
    char *operation = zmsg_popstr(msg);
    wr[operation].push_back(zframe_dup(id));
  }
  if(strcmp(op,"answer") == 0){
    cout<<"respuesta"<<endl;
    zmsg_print(msg);
    zmsg_send(&msg,clients);
  }
  free(op);
  zframe_destroy(&id);
  cout<<"Mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}


/******* Clientes *********/

void handleClient(zmsg_t *msg, void *workers){
  cout<< "Enviando a los workers :"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  //IMPORTANTE
  cout<<op<<endl;
  zframe_t* worker = getWorker(op);
  zmsg_pushstr(msg, op);
  zmsg_prepend(msg, &id);
  zmsg_prepend(msg, &worker);
  cout<<"aqui"<<endl;
  zmsg_print(msg);
  zmsg_send(&msg,workers);
  free(op);
  cout<<"Mensaje enviado"<<endl;
  zmsg_destroy(&msg);

}

int main(void) {
  zctx_t* context = zctx_new();
  void* clients = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(clients, "tcp://*:4444");
  void* workers = zsocket_new(context,ZMQ_ROUTER);
  zsocket_bind(workers, "tcp://*:5555");

  zmq_pollitem_t items[] = {{workers,0,ZMQ_POLLIN,0},
                            {clients,0,ZMQ_POLLIN,0}};

  cout<<"Estamos listos!"<<endl;

  while(true) {
    zmq_poll(items, 2,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN){
      cout<<"Mensaje de worker recibido"<<endl;
      zmsg_t *inmsg = zmsg_recv(workers);
      handleWorker(inmsg, clients);
      cout<<"Mensaje de worker despachado"<<endl;
    }
    if(items[1].revents & ZMQ_POLLIN){
      cout<<"Mensaje del cliente recibido"<<endl;
      zmsg_t *inmsg = zmsg_recv(clients);
      handleClient(inmsg,workers);
      cout<<"Mensaje del cliente despachado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}
