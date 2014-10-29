#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

map<string, vector<zframe_t*> > wr;
map<string, vector<bool> > fr;
queue<zmsg_t *> qMsg;

void parser(string &dir, string &lista){
  size_t pos = lista.find(";",0),ant = 0;
  string tmp;
  while(pos != string::npos){
    tmp = lista.substr(ant,pos);
    wr[tmp].push_back(dir);
    fr[tmp].push_back(true);
    ant = pos+1;
    pos = lista.find(";",ant);
  }
}

zframe_t * getWorker(string cancion,int pos){
  zframe_t *id = wr[cancion][pos];
  fr[cancion][pos] = false;
  return zframe_dup(id);
}

int searchPos(string cancion){
  for(int i = 0; i < wr[cancion].size(); i++)
    if(fr[cancion][i]) return i;
  return -1;
}

/******* WORKERS ***********/

void handleWorker(zmsg_t *msg, void *clients){
  cout<<"Enviando a los clientes"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  if(strcmp(op,"reportandoce") == 0){
    string lista = zmsg_popstr(msg), dir = zframe_strhex(id);
    canciones += lista;
    parser(dir, lista);
  }
  if(strcmp(op,"cancion") == 0){
    //arreglar
    zmsg_print(msg);
    zmsg_send(&msg,clients);
  }
  zframe_destroy(&id);
  cout<<"Mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}


/******* Clientes *********/

void handleClient(zmsg_t *msg){
  cout<< "Encolando mensajes :"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *cancion = zmsg_popstr(msg);

  zmsg_prepend(msg, &id);
  zmsg_pushstr(msg, cancion);
  //zmsg_prepend(msg, &worker);
  zmsg_print(msg);
  qMsg.push(msg);
  //zmsg_send(&msg,workers);
  free(cancion);
}

void sendToWorker(void *workers){
  cout<<"Tratando de enviar a los workers"<<endl;
  zmsg_t *msg = zmsg_dup(qMsg.front());
  string cancion = zmsg_popstr(msg);

  int pos = searchPos(cancion);

  if(pos < 0) {
    zmsg_destroy(&msg);
    return;
  }
  zframe_t* worker = getWorker(cancion,pos);
  char *tmp = new char;
  sprintf(tmp," %i",pos);
  cancion += tmp;
  qMsg.pop();

  zmsg_addstr(msg,cancion.c_str());
  zmsg_prepend(msg, &worker);
  zmsg_send(&msg,workers);
  cout<<"mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}

int main(void) {
  // definicion de sockets
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
      handleClient(inmsg);
      cout<<"Mensaje del cliente despachado"<<endl;
    }
    if(!qMsg.empty())   sendToWorker(workers);    
  }
  zctx_destroy(&context);
  return 0;
}
