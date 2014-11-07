#include <bits/stdc++.h>
#include <czmq.h>
#define endl '\n'
#define _dbg(x) cout<<"----------------------"<<x<<"------------------------"<<endl;


using namespace std;

/************************************************************
* parser 
* vector<string> canciones : vector con la lista de canciones
* string lista : lista de canciones recibida del broker
* parcea la cadena buscando el ";" que indica el final de
* la cancion y saca su nombre y lo pone en la lista
* que al final ordena
************************************************************/
void parser(vector<string> &canciones, string &lista){
  _dbg("parseando ando")
  size_t pos,ant = 0;
  string act;
  while(true){
    pos = lista.find(";",ant); 
    if(pos == string::npos) break;
    act = lista.substr(ant,pos-ant-4);
    if(!binary_search(canciones.begin(), canciones.end(),act) and act.find("publicidad") == string::npos)
      canciones.push_back(act);
    ant = pos+1;
  }
  sort(canciones.begin(), canciones.end());
}

/************************************************************
pedir
string pedido : cancion solicitada
void *server : socket al servidor
Agrega el pedido al mensaje y espera a que el servidor responda
y escribe el archivo
************************************************************/


void pedir(string &pedido, void *server){
  _dbg("pidiendo cancion")
  zframe_t* dato;
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,pedido.c_str());
  zmsg_print(msg);
  zmsg_send(&msg,server);
  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  _dbg("esperando cancion")
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      //para borrar las canciones despues de reproducirlas
      system("rm *.mp3");
      cout << "Ya llego la cancion patron:"<<endl;
      zmsg_t *incmsg = zmsg_recv(server);
      zmsg_print(incmsg);
      string strout = zmsg_popstr(incmsg);
      strout += ".mp3";
      zfile_t *download = zfile_new("./", strout.c_str());
      zfile_output(download);
      dato = zmsg_pop(incmsg);
      zchunk_t *chunk = zchunk_new(zframe_data(dato), zframe_size(dato)); 
      zfile_write(download, chunk, 0);
      zfile_close(download);
      zmsg_destroy(&incmsg);
      system("mplayer -slave -quiet A.mp3");
      if(strout.find("publicidad") == string::npos ) {
         _dbg(strout);
        break;
      }
    }
  }
}

/************************************************************
Main
Crea server y envia mensaje de reporte
y crea lista de canciones
************************************************************/

int main(int argc, char** argv) {
  //Creando el contexto y el socket
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);
  zsocket_connect(server, "tcp://localhost:4444");

  //Enviando mensaje para recibir la lista de musica
  string lista="";
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"a");
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
  while(true){
    cout<<"\"imprimir\" para lista de canciones, \"sacamedeaqui!\" para salir y ";
    cout<<"el nombre de la cancion para agregar a la lista de reproduccion"<<endl;
    cin>>pedido;
    if(pedido.compare("sacamedeaqui!") == 0){
      cout<<"Chao nene"<<endl;
      break;
    }
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
