#include <bits/stdc++.h>
#include <czmq.h>

#define endl '\n'

using namespace std;

void parser(vector<string> &canciones, string &lista){
  size_t pos = lista.find(";",0),ant = 0;
  string act;
  while(pos != string::npos){
    act = lista.substr(ant,pos);
    if(!binary_search(canciones.begin(), canciones.end(),act))
      canciones.push_back();
    ant = pos;
    pos = lista.find(";",pos);
  }
  sort(canciones.begin(), canciones.end());
}

void pedir(string &pedido, void *server){
  string dato;
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"pedido");
  zmsg_addstr(msg,pedido.c_str());
  zmsg_send(&msg,server);
  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      cout << "Ya llego la cancion patron:"<<endl;
      zmsg_t *incmsg = zmsg_recv(server);
      zmsg_print(incmsg);
      dato = zmsg_popstr(incmsg);
      if(dato.compare("acabo")){
        break;
      }
      //aca va lo de recibir el archivo 
      dato = zmsg_popstr(incmsg);
      zmsg_destroy(&incmsg);
    }
  }
}

int main(int argc, char** argv) {
  //Creando el contexto y el socket
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);
  zsocket_connect(server, "tcp://localhost:4444");

  //Enviando mensaje para recibir la lista de musica
  string lista="";
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"Givemepower");
  zmsg_send(&msg,server);

  cout<<"Ya le llega la lista relajese"<<endl;

  //recibiendo lista de musica
  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      cout << "Ya llego!!:"<<endl;
      zmsg_t *incmsg = zmsg_recv(server);
      zmsg_print(incmsg);
      lista = zmsg_popstr(incmsg);
      zmsg_destroy(&incmsg);
      zmsg_destroy(&msg);
      break;
    }
  }

  //parseando y ordenando lista de canciones
  vector<string> canciones;
  parser(canciones, lista);

  string pedido = "";

  while(pedido.compare("sacamedeaqui!") != 0){
    cout<<"imprimir para lista de canciones, sacamedeaqui! para salir y ";2
    cout<<"el nombre de la cancion para agregar a la lista de reproduccion"<<endl;
    cin>>pedido;
    if(pedido.compare("imprimir") == 0){
      for(int i = 0; i < canciones.size(); i++)
        cout<<canciones[i]<<endl;
      continue;
    }
    // crear un hilo que reciba el archivo
    if(binary_search(canciones.begin(),canciones.end(),pedido))
      pedir(pedido,server);
    else
      cout<<"No tenemos esa rola wey"<<endl;
  }

  //terminando la sesion
  zctx_destroy(&context);
  return 0;
}
