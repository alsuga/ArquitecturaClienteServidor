#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

vector<zframe_t *>  wr;
vector<bool> fr;
queue<zmsg_t *> qMsg;

zframe_t * getWorker(int pos){
  fr[pos] = false;
  return zframe_dup(wr[pos]);
}

int searchPos(){
  for(int i = 0; i < fr.size(); i++){
    if(fr[i]) return i;
  }
  return -1;
}

/******* WORKERS ***********/

void handleWorker(zmsg_t *msg, void *clients){
  cout<<"Mensaje de workers"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  if(strcmp(op,"reporting") == 0){
    wr.push_back(zframe_dup(id));
    fr.push_back(true);
    cout<<"Worker reportado"<<endl;
  }
  if(strcmp(op,"answer") == 0){

    char *position = zmsg_popstr(msg);
    int pos;
    pos= atoi(position);
    fr[pos] = true;
    zmsg_print(msg);
    zmsg_send(&msg,clients);
    cout<<"Mensaje enviado"<<endl;
  }
  free(op);
  zframe_destroy(&id);
  zmsg_destroy(&msg);
}


/******* Clientes *********/

void handleClient(zmsg_t *msg){
  // repartir mensajes
  cout<< "Encolando mensajes :"<<endl;
  zframe_t *id = zmsg_pop(msg);
  string st = zmsg_popstr(msg);
  stringstream ss(st);
  int m,n,o; ss>>m>>n>>o;
  char *two = zmsg_popstr(msg);
  for(int i = 0; i < m; i++){
    zmsg_t *n = zmsg_new();
    zframe_t *dup = zframe_dup(id);
    zmsg_addstr(n,st.c_str());
    zmsg_addstr(n,two);
    zmsg_addstr(n,zmsg_popstr(msg));
    zmsg_pushstr(n,to_string(i).c_str());
    zmsg_prepend(n,&dup);
    zmsg_print(n);
    qMsg.push(n);
  }
  zmsg_destroy(&msg);
}

void sendToWorker(void *workers){
  cout<<"Tratando de enviar a los workers"<<endl;
  zmsg_t *msg = zmsg_dup(qMsg.front());

  int pos = searchPos();
  if(pos < 0) {
    zmsg_destroy(&msg);
    cout<<"no hay disponibles"<<endl;
    return;
  }
  zframe_t* worker = getWorker(pos);
  qMsg.pop();
  zmsg_pushstr(msg,to_string(pos).c_str());
  zmsg_prepend(msg, &worker);
  cout<<"antes de enviar esto le tengo"<<endl;
  zmsg_print(msg);
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
    if(!qMsg.empty())
      sendToWorker(workers);
  }
  zctx_destroy(&context);
  return 0;
}
