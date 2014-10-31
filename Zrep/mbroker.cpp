#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

map<string, vector<zframe_t*> > wr;
map<string, vector<bool> > fr;
map<string, vector<string> > mus;
vector<zframe_t*> dirwork;
queue<zmsg_t *> qMsg;

/*
void parser(vector<string> &canciones, string &lista){
  size_t pos,ant = 0;
  string act;
  while(true){
    pos = lista.find(";",ant); 
    if(pos == string::npos) break;
    act = lista.substr(ant,pos-ant);
    if(!binary_search(canciones.begin(), canciones.end(),act))
      canciones.push_back(act);
    ant = pos+1;
  }
  sort(canciones.begin(), canciones.end());
}
*/

zframe_t* getDir(string name){
  for(auto it = mus.begin();  it != mus.end(); ++it ){
    if(!binary_search(it->second.begin(), it->second.end(),name)){
      for(auto ite = dirwork.begin(); ite != dirwork.end(); ++ite){
        if(strcmp(zframe_strhex(*ite), zframe_strhex(it->first.c_str()) ) ) return *ite;
      }
    }
  }
  return dirwork[rand()%dirwork.size()];
}

void parser(zframe_t *dir, string &lista){
  size_t pos,ant = 0;
  string act;
  dirwork.push_back(dir);
  while(true){
    pos = lista.find(";",ant); 
    if(pos == string::npos) break;
    act = lista.substr(ant,pos-ant);
    mus[zframe_strhex(dir)].push_back(act);
    wr[act].push_back(dir);
    fr[act].push_back(true);
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

void handleWorker(zmsg_t *msg, void *clients,void *workers){
  cout<<"Enviando a los clientes"<<endl;
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  if(strcmp(op,"reportandoce") == 0){
    string lista = zmsg_popstr(msg);
    canciones += lista;
    parser(id, lista);
  }
  if(strcmp(op,"cancion") == 0){
    string cancion = zmsg_popstr(msg);
    zmsg_print(msg);
    zmsg_send(&msg,clients);
  }
  if(strcmp(op, "replica") == 0){
    string cancion = zmsg_popstr(msg);
    zframe_t *dir = getDir(cancion);
    zmsg_pushstr(msg,cancion.c_str());    
    zmsg_pushstr(msg,"replica");
    zmsg_prepend(msg,dir);
    zmsg_send(&msg,workers);
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
  string cancion = zmsg_popstr(msg);

  zmsg_prepend(msg, &id);
  zmsg_pushstr(msg, cancion.c_str());
  //zmsg_prepend(msg, &worker);
  
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
 /* char *tmp = new char;
  sprintf(tmp," %i",pos);
  cancion += tmp;
  */
  qMsg.pop();

  zmsg_addstr(msg,cancion.c_str());
  zmsg_addstr(msg,"cancion");
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
      handleWorker(inmsg, clients,workers);
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
