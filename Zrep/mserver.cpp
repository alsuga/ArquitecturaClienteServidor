#include <bits/stdc++.h>
#include <dirent.h>
#include <czmq.h>
#include <thread>
#define _dbg(x) cout<<"----------------------"<<x<<"------------------------"<<endl;


using namespace std;

// map<string,int> uso : sirve para saber si es necesario
// replicar una cancion
map<string,int> uso;


/************************************************************
enviarCancion
string cancion: cancion a enviar
zmsg_t* msg : mensaje en donde se empaqueta la cancion
void* server : socket al broker
agrega la cancion al mensaje, luego saca el archivo, lo 
empaqueta en un chunk, este en un frame y se envia
************************************************************/

void enviarCancion(string cancion,zmsg_t *msg, void *server){

  cout<< "Solicitada "<<cancion<<endl;
  zmsg_addstr(msg,cancion.c_str());
  cancion += ".mp3";
  _dbg(cancion);
  string a = "./canciones/";
  a+= cancion;
  zchunk_t *chunk = zchunk_slurp(a.c_str(),0);
  if(!chunk) {
    cout << "no se puede leer!" << endl;
    return;
  }
  zframe_t *frame = zframe_new(zchunk_data(chunk), zchunk_size(chunk));
  zmsg_append(msg,&frame);
  zmsg_send(&msg,server);
}

/************************************************************
reproducida
string cancion : cancion a reproducir
void* server : socket al broker
cuenta una reproduccion de la cancion, si tiene las suficientes
(10) se genera una replica
************************************************************/

void reproducida(string &cancion, void *server){
  uso[cancion]++;
  if(uso[cancion] > 0){
    uso[cancion] = 0;
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg,"replica");
    //thread t(enviarCancion,cancion,msg,server);
    _dbg("replica")
    enviarCancion(cancion,msg,server);
  }
}


/************************************************************
listar
void* server : socket al broker
lee los archivos en un directorio
y los lista, ordena y envia al broker
************************************************************/

int listar(void *server) {
  string canciones = "";
  DIR *dir;
  struct dirent *ent;
  vector<string> vec;
  if ((dir = opendir ("canciones")) != NULL) {

    while ((ent = readdir (dir)) != NULL) {
      if( strcmp(ent->d_name,".") != 0 and strcmp(ent->d_name, "..") != 0){
        vec.push_back(ent->d_name);
        uso[ent->d_name];
      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    return 0;
  }
  sort(vec.begin(), vec.end());
  for(int i = 0; i < vec.size(); i++){
      canciones += vec[i];
      canciones += ';'; 
  }
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"reportandoce");
  zmsg_addstr(msg, canciones.c_str());
  //_dbg("listar");
  //zmsg_print(msg);
  zmsg_send(&msg,server);
  return 1;
}

/************************************************************
dispatcher
zmsg_t* in_msg : mensaje recibido del broker
void* server: socket al broker
Se desempaqueta el mensaje, se evalua si pide una cancion,
si lo hace se revisa si es momento de enviar una pauta 
publicitaria y si lo es se envia y luego la cancion.
si se pide una replica se envia el archivo solicitado
************************************************************/


void dispatcher(zmsg_t *in_msg, void *server){
  zmsg_t *out_msg = zmsg_new();
  zframe_t *wh = zmsg_pop(in_msg);
  _dbg("");
  zmsg_print(in_msg);
  string a = zmsg_popstr(in_msg);
  if(a.compare("cancion") == 0){
    a = zmsg_popstr(in_msg);
    if(a.find("publicidad") == string::npos){
      reproducida(a,server);
    }else{
      a += to_string(rand() %uso.size());
    }
    zmsg_addstr(out_msg,"cancion");
    zmsg_append(out_msg, &wh);
    enviarCancion(a,out_msg,server);
  }
  if(a.compare("replica") == 0){
    _dbg("replica recibida")
    zmsg_print(in_msg);
    a = zmsg_popstr(in_msg);
    a += ".mp3";
    zfile_t *download = zfile_new("./canciones/", a.c_str());
    zfile_output(download);
    zframe_t *filePart = zmsg_pop(in_msg);
    zchunk_t *chunk = zchunk_new(zframe_data(filePart), zframe_size(filePart)); 
    zfile_write(download, chunk, 0);
    zfile_close(download);
  }
  zframe_destroy(&wh);
}

/************************************************************
main
crea y conecta el socket, lista sus canciones y las envia al
broker reportandoce, luego se queda esperando si se necesita
trabajo.
************************************************************/

int main(){
  srand (time(NULL));
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context, ZMQ_DEALER);
  int c = zsocket_connect(server, "tcp://localhost:5555");
  cout << "connecting to server: " << (c == 0 ? "OK" : "ERROR") << endl;

  cout<<"Reportandoce!"<<endl;
  
  if(listar(server) ) cout<<"Reportado!"<<endl;
  else{
    cout<<"Error listando"<<endl;
    return 0;
  }

  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};

  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      cout << "Tengo trabajo!!"<<endl;
      zmsg_t *incmsg = zmsg_recv(server);
      
      zmsg_print(incmsg);
      dispatcher(incmsg,server);
      zmsg_destroy(&incmsg);
      cout<<"Enviado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}
