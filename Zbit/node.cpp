#include<bits/stdc++.h>
#include<czmq.h>


#define SIZEOFPART 524288

using namespace std;

/***********************
 * Variables Globales
 **********************/
string myip;


void parser(vector<string> &canciones, string &lista);
void connectNode(void *node,string dirNode);
void report(zmsg_t *msg);

int main(int argc, const char *argv[]) {
  //Conexiones
  if(argc > 1)
    myip = argv[1];
  else
    myip = "localhost";
  string dir = "tcp://";
  if(argc > 2)
    dir += argv[1];
  else
    dir += "localhost";
  dir += ":";
  if(argc > 3)
    dir += argv[2];
  else
    dir += "4444";
  zctx_t *context = zctx_new();
  void *tracker = zsocket_new(context, ZMQ_DEALER);
  zsocket_connect(tracker,dir.c_str());
  void *listen = zsocket_new(context,ZMQ_REP);
  zsocket_bind(listen,"tcp://*:5555");
 
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"report");
  zmsg_addstr(msg,myip.c_str());
  report(msg);
  zmsg_send(&msg,tracker);

  zctx_destroy(&context);
  return 0;
}

void dispatchSong(zmsg_t *request, zmsg_t *response){
  string name = zmsg_popstr(request);
  string part =zmsg_popstr(request);
  zmsg_addstr(response,name.c_str());
  zmsg_addstr(response,part.c_str());

  name+=part;
  name+=".mp3";
  string a = "./canciones/";
  a+= name;
  zchunk_t *chunk = zchunk_slurp(a.c_str(),0);
  if(!chunk) {
    cout << "no se puede leer!" << endl;
    return;
  }
  zframe_t *frame = zframe_new(zchunk_data(chunk), zchunk_size(chunk));
  zmsg_append(response,&frame);
}

void listening(void *listen){
  while(1){
    zmsg_t* request = zmsg_recv(listen);
    zmsg_t* response = zmsg_new();
    dispatchSong(request,response);
    zmsg_send(&response,listen);
  }
}

/****************************
 * Report
 *
 ****************************/


void report(zmsg_t *msg){
  string canciones = "";
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ("canciones")) != NULL) {
    while ((ent = readdir (dir)) != NULL)
      if( strcmp(ent->d_name,".") != 0 and strcmp(ent->d_name, "..") != 0){
        zmsg_addstr(msg,ent->d_name);
      }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
  }
  //_dbg("listar");
  //zmsg_print(msg);
}

/************************************************************
* parser
* vector<string> canciones : vector con la lista de canciones
* string lista : lista de canciones recibida del broker
* parcea la cadena buscando el ";" que indica el final de
* la cancion y saca su nombre y lo pone en la lista
* que al final ordena
************************************************************/
void parser(vector<string> &canciones, string &lista){
  size_t pos,ant = 0;
  string act;
  while(true){
    pos = lista.find(";",ant);
    if(pos == string::npos) break;
    act = lista.substr(ant,pos-ant-4);
    if(!binary_search(canciones.begin(), canciones.end(),act))
    canciones.push_back(act);
    ant = pos+1;
  }
  sort(canciones.begin(), canciones.end());
}

/**********************************************************
 * connectNode
 *********************************************************/

void *connectNode(zctx_t *context,string dirNode){
  void *node = zsocket_new(context,ZMQ_REQ);
  zsocket_connect(node,dirNode.c_str());
  return node;
}
