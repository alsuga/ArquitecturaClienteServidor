#include <bits/stdc++.h>
#include <czmq.h>

#define _dbg(x) cout<<"----------------------"<<x<<"------------------------"<<endl;

using namespace std;

map<string, vector<zframe_t*> > wr;
map<string, bool > fr;
map<string, vector<string> > mus;
map<string,int> clnts;
vector<zframe_t*> dirwork;
queue<zmsg_t *> qMsg;
string canciones;

zframe_t* getDir(string name){
  for(auto it = mus.begin();  it != mus.end(); ++it ){
    if(!binary_search(it->second.begin(), it->second.end(),name)){
      for(auto ite = dirwork.begin(); ite != dirwork.end(); ++ite){
        if(strcmp(zframe_strhex(*ite), it->first.c_str() ) ) return *ite;
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
    act = lista.substr(ant,pos-ant-4);
    mus[zframe_strhex(dir)].push_back(act);
    wr[act].push_back(dir);
    act += zframe_strhex(dir);
    fr[act] = true;
    ant = pos+1;
    pos = lista.find(";",ant);
  }
}

zframe_t * getWorker(string cancion,int pos){
  zframe_t *id = wr[cancion][pos];
  cancion += zframe_strhex(id);
  fr[cancion] = false;
  return zframe_dup(id);
}

int searchPos(string cancion){
  _dbg("Buscando posicion");
  string a,tmp;
  //_dbg((wr[cancion][0]))
  for(int i = 0; i < wr[cancion].size(); ++i){
    a = cancion;
    cout<<i<<endl;
    tmp = zframe_strhex(wr[cancion][i]);
    a+= tmp;
    if(fr[a]) return i;
  }
  return -1;
}

/******* WORKERS ***********/

void handleWorker(zmsg_t *msg, void *clients,void *workers){
  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  zmsg_print(msg);
  if(strcmp(op,"reportandoce") == 0){
    _dbg("Worker reportandoce")
    string lista = zmsg_popstr(msg);
    canciones += lista;
    parser(id, lista);
  }
  if(strcmp(op,"cancion") == 0){
    _dbg("cancion para el cliente")
    zmsg_print(msg);
    zmsg_send(&msg,clients);
  }
  if(strcmp(op, "replica") == 0){
    _dbg("worker pidiendo replica")
    string cancion = zmsg_popstr(msg);
    zframe_t *dir = getDir(cancion);
    zmsg_pushstr(msg,cancion.c_str());    
    zmsg_pushstr(msg,"replica");
    zmsg_prepend(msg,&dir);
    zmsg_send(&msg,workers);
  }
  cout<<"Mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}


/******* Clientes *********/

void handleClient(zmsg_t *msg, void * clients){
  _dbg("encolando mensaje")
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  string tmp = zframe_strhex(id);
  if(clnts[tmp] == 0){
    _dbg("activando nuevo cliente");
    clnts[tmp] = 1;
    _dbg(canciones)
    zmsg_popstr(msg);
    zmsg_pushstr(msg,canciones.c_str());
    zmsg_prepend(msg, &id);
    zmsg_send(&msg,clients);
    return;
  }
  if(clnts[tmp] == 3){
    _dbg("enviando publicidad");
    clnts[tmp] = 0;
    zmsg_t *publ = zmsg_dup(msg);
    zmsg_popstr(publ);
    zmsg_prepend(publ, &id);
    zmsg_pushstr(msg, "publicidad");
    qMsg.push(msg);
  }
  _dbg("pidinedo cancion")
  string cancion = zmsg_popstr(msg);

  zmsg_prepend(msg, &id);
  zmsg_pushstr(msg, cancion.c_str());
  //zmsg_prepend(msg, &worker);
  
  qMsg.push(msg);
  clnts[tmp]++;
  //zmsg_send(&msg,workers);
}

void sendToWorker(void *workers){
  _dbg("tratando de enviar a los workers")
  zmsg_t *msg = zmsg_dup(qMsg.front());
  zmsg_print(msg);
  string cancion = zmsg_popstr(msg);

  int pos = searchPos(cancion);

  if(pos < 0) {
    _dbg("Donde estas?")
    zmsg_destroy(&msg);
    return;
  }
  _dbg("se fue")
  zframe_t* worker = getWorker(cancion,pos);
 /* char *tmp = new char;
  sprintf(tmp," %i",pos);
  cancion += tmp;
  */
  qMsg.pop();
  zmsg_addstr(msg,"cancion");
  zmsg_addstr(msg,cancion.c_str());
  zmsg_prepend(msg, &worker);
  zmsg_print(msg);
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
      handleClient(inmsg,clients);
      cout<<"Mensaje del cliente despachado"<<endl;
    }
    if(!qMsg.empty()) sendToWorker(workers);  
  }
  zctx_destroy(&context);
  return 0;
}
