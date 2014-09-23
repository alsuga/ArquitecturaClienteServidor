#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

map<string, vector<zframe_t*> > wr;
map<string, vector<bool> > fr;
queue<zmsg_t *> qMsg;

zframe_t * getWorker(string operation,int pos){
  zframe_t *id = wr[operation][pos];
  fr[operation][pos] = false;

  return zframe_dup(id);
}

int searchPos(string op){
  for(int i = 0; i < wr[op].size(); i++)
    if(fr[op][i]) return i;
  return -1;
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
    fr[operation].push_back(true);
  }
  if(strcmp(op,"answer") == 0){

    char *positions = zmsg_popstr(msg);
    char *oper = new char;
    int pos;
    sscanf(positions,"%s %i",oper,&pos);
    fr[oper][pos] = true;
    zmsg_print(msg);
    zmsg_send(&msg,clients);
  }
  free(op);
  zframe_destroy(&id);
  cout<<"Mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}


/******* Clientes *********/

void handleClient(zmsg_t *msg){
  cout<< "Encolando mensajes :"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);

  zmsg_prepend(msg, &id);
  zmsg_pushstr(msg, op);
  //zmsg_prepend(msg, &worker);
  zmsg_print(msg);
  qMsg.push(msg);
  //zmsg_send(&msg,workers);
  free(op);
}

void sendToWorker(void *workers){
  cout<<"Tratando de enviar a los workers"<<endl;
  zmsg_t *msg = zmsg_dup(qMsg.front());
  string op = zmsg_popstr(msg);

  int pos = searchPos(op);

  if(pos < 0) {
    zmsg_destroy(&msg);
    return;
  }
  zframe_t* worker = getWorker(op,pos);
  char *tmp = new char;
  sprintf(tmp," %i",pos);
  op += tmp;
  qMsg.pop();

  zmsg_addstr(msg,op.c_str());
  zmsg_prepend(msg, &worker);
  zmsg_send(&msg,workers);
  cout<<"mensaje enviado"<<endl;
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
      handleClient(inmsg);
      cout<<"Mensaje del cliente despachado"<<endl;
    }
    if(!qMsg.empty())   sendToWorker(workers);    
  }
  zctx_destroy(&context);
  return 0;
}
