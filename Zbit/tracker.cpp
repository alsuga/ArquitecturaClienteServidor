#include <bits/stdc++.h>
#include <czmq.h>

#define PARTSIZE 524288
#define endl '\n'
#define _dbg(x) if(1) cout<<"----------------------"<<x<<"------------------------"<<endl;


using namespace std;

// Crear estructura de datos para guardar quien tiene las partes
map<string,vector< vector<string> > > info;


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
      handler(inmsg,clients);
      cout<<"Mensaje de worker despachado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}

/*****************************************
* FUNCIONES!
*****************************************/


void handler(zmsg_t *msg,void * clients){
  zframe_t *dir = zmsg_pop(msg);
  string op = zmsg_popstr(msg);
  if(op == "report"){
    string client = zmsg_popstr(msg),song;
    int nparts;
    while(zmsg_size(msg) > 0){
      song = zmsg_popstr(msg);
      nparts = atoi(zmsg_popstr(msg));
      if(info[song].size() == 0) info[song].resize(nparts);
      for(int i = 0; i < nparts; i++){
        info[song][i].push_back(client);
      }
    }
  }
  if(op == "request"){
    string song = zmsg_popstr(msg);
    zmsg_addstr(msg, to_string(info[song].size()));
    for(int i = 0; i < info[song].size(); i++){
      zmsg_addstr(msg,"**");
      for(int j = 0; j < info[song][i].size(); i++){
        zmsg_addstr(msg,info[song][i][j]);
      }
    }
    zmsg_prepend(msg,&dir);
    zmsg_send(&msg,clients);
  }

  if(op == "npart"){
    string client = zmsg_popstr(msg),song;
    int part;
    song = zmsg_popstr(msg);
    part = atoi(zmsg_popstr(msg));
    info[song][part].push_back(client);
  }

  if(op = "retire"){
    string client = zmsg_popstr(msg),song;
    int nparts;
    vector<string>::iterator it;
    while(zmsg_size(msg) > 0){
      song = zmsg_popstr(msg);
      for(int i = 0; i < info[song].size() ; i++){
        it = find(info[song][i].begin(),info[song][i].end(),client);
        if(it != info[song][i].end()) info[song][i].erase(it);
      }
    }
  }
}
