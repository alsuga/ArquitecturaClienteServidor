#include <bits/stdc++.h>
#include <dirent.h>
#include <czmq.h>

using namespace std;

map<string,int> uso;

void replicar(string cancion){
  cout<<"en construccion"<<endl;
}

void reproducida(string cancion){
  map[cancion]++;
  if(map[cancion] > 10){
    map[cancion] = 0;
    replicar(cancion);
  }
}

int listar(void *server) {
  string canciones = "";
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ("canciones")) != NULL) {

    while ((ent = readdir (dir)) != NULL) {
      if( strcmp(ent->d_name,".") != 0 and strcmp(ent->d_name, "..") != 0){
        canciones += ent->d_name;
        canciones += ';'; 
      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    return 0;
  }
  zmsg_t msg = zmsg_new();
  zmsg_addstr(msg,"reportandoce");
  zmsg_addstr(msg, canciones.c_str());
  zmsg_send(&msg,server);
  return 1;
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  zmsg_print(in_msg);
  string a = zmsg_popstr(in_msg);
  a+= "prueba";
  //partir canciones
  zmsg_addstr(out_msg,"cancion");
  zmsg_append(out_msg, &wh);
  zmsg_addstr(out_msg,a.c_str());
  //enviar mensaje en un while
  //enviar fin de canciones
  zframe_destroy(&wh);
}

int main(){
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
      zmsg_t *outmsg = zmsg_new();
      
      zmsg_print(incmsg);

      dispatcher(incmsg,outmsg);
      zmsg_destroy(&incmsg);
      zmsg_print(outmsg);
      zmsg_send(&outmsg,server);
      cout<<"Enviado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}
