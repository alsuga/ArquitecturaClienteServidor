#include <bits/stdc++.h>
#include <czmq.h>

#define _dbg(x) cout<<"----------------------"<<x<<"------------------------"<<endl;

using namespace std;

/************************************************************
map<string, vector<zframe_t*> > wr : Sirve para saber en que 
servidores de musica se encuentra una cancion.

map<string, bool > fr : como llave tiene concatenado el nombre
de la cancion con la direccion del worker para saber de manera
sencilla si el worker con la cancion esta ocupado.

map<string, vector<string> > mus : sirve para indicar si un
servidor de musica tiene una cancion, lo cual ayuda a indicar
a que servidor se debe replicar.

map<string,int> clnts : Indica el numero de canciones que
ha pedido un cliente con el fin de determinar si se debe enviar
publicidad

vector<zframe_t*> dirwork : Lista con las direcciones de los
servidores de musica

queue<zmsg_t *> qMsg : Cola donde se guardan los mensajes
para ser enviados a los servidores de musica
string canciones :
************************************************************/
map<string, vector<zframe_t*> > wr;
map<string, bool > fr;
map<string, vector<string> > mus;
map<string,int> clnts;
vector<zframe_t*> dirwork;
queue<zmsg_t *> qMsg;
string canciones;

/************************************************************
string name : Si un servidor no tiene una cancion se elige 
este para poner la replica de la cacion alli 
************************************************************/

zframe_t* getDir(string &name){
  //_dbg(name)
  for(auto it = mus.begin();  it != mus.end(); ++it ){
    if(!binary_search(it->second.begin(), it->second.end(),name)){
      for(auto ite = dirwork.begin(); ite != dirwork.end(); ++ite){
        //_dbg(zframe_strhex(*ite))
        //_dbg(it->first.c_str());
        if(strcmp(zframe_strhex(*ite), it->first.c_str() ) == 0 ) return *ite;
      }
    }
  }
  //_dbg("get dir out")
  return dirwork[rand()%dirwork.size()];
}

/************************************************************
parser
zframe_t *dir : 
string &lista :
************************************************************/

void parser(zframe_t *dir, string &lista){
  size_t pos,ant = 0;
  string act;
  bool fg = false;
  while(true){
    pos = lista.find(";",ant); 
    if(pos == string::npos) break;
    act = lista.substr(ant,pos-ant-4);
    if(act.find("publicidad") != string::npos){
      fg = true;
      act = "publicidad";
    }
    mus[zframe_strhex(dir)].push_back(act);
    zframe_t* tmp = zframe_dup(dir);
    wr[act].push_back(tmp);
    act += zframe_strhex(dir);
    fr[act] = true;
    if(fg) return;
    ant = pos+1;
    pos = lista.find(";",ant);
  }
  zframe_t* tmp = zframe_dup(dir);
  dirwork.push_back(tmp);
}

/************************************************************
************************************************************/

zframe_t * getWorker(string cancion,int pos){
  zframe_t *id = wr[cancion][pos];
  _dbg("marco")
  _dbg(cancion)
  cancion += zframe_strhex(id);
  fr[cancion] = false;
  return zframe_dup(id);
}

/************************************************************
************************************************************/

int searchPos(string cancion){
  _dbg("Buscando posicion");
  string a,tmp;
  //_dbg((wr[cancion][0]))
  for(int i = 0; i < wr[cancion].size(); ++i){
    a = cancion;
    cout<<i<<" ";
    tmp = zframe_strhex(wr[cancion][i]);
    cout<<tmp<<endl;
    a+= tmp;
    _dbg(a)
    _dbg(fr[a])
    if(fr[a]) return i;
  }
  return -1;
}

/************************************************************
************************************************************/

void handleWorker(zmsg_t *msg, void *clients,void *workers){
  zframe_t* id = zmsg_pop(msg);
  char *op = zmsg_popstr(msg);
  //zmsg_print(msg);
  if(strcmp(op,"reportandoce") == 0){
    _dbg("Worker reportandoce")
    string lista = zmsg_popstr(msg);
    canciones += lista;
    parser(id, lista);
  }
  if(strcmp(op,"cancion") == 0){
    _dbg("cancion para el cliente, desmarco")
    zframe_t* tmp =  zmsg_pop(msg);
    string a = zmsg_popstr(msg),b = a;
    _dbg(a)
    a += zframe_strhex(id);
    //_dbg(a)
    fr[a] = true;
    zmsg_pushstr(msg,b.c_str());
    zmsg_prepend(msg,&tmp);
    //zmsg_print(msg);
    zmsg_send(&msg,clients);
  }
  if(strcmp(op, "replica") == 0){
    _dbg("worker pidiendo replica")
    zmsg_print(msg);
    string cancion = zmsg_popstr(msg);
    zframe_t *dir = getDir(cancion);
    zmsg_pushstr(msg,cancion.c_str());    
    zmsg_pushstr(msg,"replica");
    zmsg_prepend(msg,&id);
    zmsg_prepend(msg,&dir);
    //_dbg("salida")
    zmsg_print(msg);
    zmsg_send(&msg,workers);
  }
  cout<<"Mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}


/************************************************************
************************************************************/

void handleClient(zmsg_t *msg, void * clients){
  _dbg("encolando mensaje")
  zmsg_print(msg);

  zframe_t* id = zmsg_pop(msg);
  string tmp = zframe_strhex(id);
  if(clnts[tmp] == 0){
    _dbg("activando nuevo cliente");
    clnts[tmp] = 1;
    //_dbg(canciones)
    zmsg_popstr(msg);
    zmsg_pushstr(msg,canciones.c_str());
    zmsg_prepend(msg, &id);
    zmsg_send(&msg,clients);
    return;
  }
  if(clnts[tmp] == 2){
    _dbg("enviando publicidad");
    clnts[tmp] = 0;
    zmsg_t *publ = zmsg_dup(msg);
    zmsg_popstr(publ);
    zframe_t* tmp = zframe_dup(id);
    zmsg_prepend(publ, &tmp);
    zmsg_pushstr(publ, "publicidad");
    zmsg_print(publ);
    qMsg.push(publ);
  }
  _dbg("pidinedo cancion")
  zmsg_print(msg);
  string cancion = zmsg_popstr(msg);

  zmsg_prepend(msg, &id);
  zmsg_pushstr(msg, cancion.c_str());
  //zmsg_prepend(msg, &worker);
  //zmsg_print(msg);
  qMsg.push(msg);
  clnts[tmp]++;
  //zmsg_send(&msg,workers);
}

/************************************************************
************************************************************/

void sendToWorker(void *workers){
  _dbg("tratando de enviar a los workers")
  zmsg_t *msg = zmsg_dup(qMsg.front());
  zmsg_print(msg);
  string cancion = zmsg_popstr(msg);

  int pos = searchPos(cancion);

  if(pos < 0) {
    _dbg("Donde estas?")
    // temporal!!!!
    qMsg.pop();
    zmsg_destroy(&msg);
    return;
  }
  _dbg("se fue")
  zframe_t* worker = getWorker(cancion,pos);

  qMsg.pop();
  zmsg_addstr(msg,"cancion");
  zmsg_addstr(msg,cancion.c_str());
  zmsg_prepend(msg, &worker);
  zmsg_print(msg);
  zmsg_send(&msg,workers);
  cout<<"mensaje enviado"<<endl;
  zmsg_destroy(&msg);
}

/************************************************************
************************************************************/

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
  while(true ) {
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
