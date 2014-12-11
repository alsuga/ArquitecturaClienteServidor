#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <czmq.h>
#include <thread>
#include <mutex>
#include <set>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <bits/stdc++.h>


#define PARTSIZE 524288
#define endl '\n'
#define _dbg(x) if(1) cout<<"----------------------"<<x<<"------------------------"<<endl;

using namespace std;

/***********************
 * Variables Globales
 **********************/
string myip;
vector<string> mysongs;
vector<string> playlist;
int can = 1;
mutex mt;


/***********************
 * Funciones
 **********************/
void dispatchSong(zmsg_t *request, zmsg_t *response);
void listening(void *listen);
void report(zmsg_t *msg);
void requestTracker(zctx_t *context, string song, void *tracker);
void connectNode(zctx_t *context,string dirNode,string song, int part,void *tracker, set<string> &act);
void retire(void *tracker);
int partir(string name);
bool compare(vector<string> &a,vector<string> &b);


/***********************
 * MAIN
 **********************/

int main(int argc, const char *argv[]) {
  //Conexiones
  system("mocp -x > /dev/null; mocp -S > /dev/null; mocp -c > /dev/null");
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
  //zmsg_print(msg);
  zmsg_send(&msg,tracker);

  //MENU
  thread t(listening,listen);


  string pedido = "";
  int x = 0;
  while(true){
    cout<<"\"agregar\"  agregar a la lista de reproduccion, \"desconectar\" para salir y ";
    cout<<"\"play\" para reproducir la lista"<<endl;
    cin>>pedido;
    if(pedido.compare("agregar") == 0){
      string ns; cin>>ns;
      playlist.push_back(ns);
      continue;
    }
    if(pedido.compare("desconectar") == 0){
      system("mocp -x");
      retire(tracker);
      cout<<"Chao"<<endl;
      break;
    }
    if (pedido.compare("play") == 0) {
      system("moc -s; moc -c");
      //pedir cancion

      vector<string>::iterator it;
      it = find(mysongs.begin(), mysongs.end(), playlist[x]);
      if(it != mysongs.end() and *it != playlist[x])
        requestTracker(context,playlist[x],tracker);
      string cn = "rm canciones/";
      cn += playlist[x];
      //system(cn.c_str());
      cn = "mocp -a canciones/";
      cn += playlist[x].substr(0,playlist[x].size()-4);
      cn +="*";
      _dbg(cn);
      system(cn.c_str());
      system("mocp -p");
      x = (x+1) % playlist.size();
      continue;
    }
    system("clear");
  }

  zctx_destroy(&context);
  return 0;
}


/**************************************
* COMPARTIENDO CANCIONES
**************************************/

void dispatchSong(zmsg_t *request, zmsg_t *response){
  cout<<"entro aca"<<endl;
  string name = zmsg_popstr(request);
  string part =zmsg_popstr(request);
  //zmsg_addstr(response,name.c_str());
  //zmsg_addstr(response,part.c_str());
  name = name.substr(0,name.size()-4);
  if(atoi(part.c_str()) < 10) name += "0";
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
  while(can){
    zmsg_t* request = zmsg_recv(listen);
    zmsg_print(request);
    zmsg_t* response = zmsg_new();
    dispatchSong(request,response);
    zmsg_send(&response,listen);
  }
}



/****************************
 * Report
 *****************************/


void report(zmsg_t *msg){
  string canciones;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ("canciones")) != NULL) {
    while ((ent = readdir (dir)) != NULL)
      if( strcmp(ent->d_name,".") != 0 and strcmp(ent->d_name, "..") != 0){
        mysongs.push_back(ent->d_name);
        zmsg_addstr(msg,ent->d_name);
        string tmp = to_string(partir(ent->d_name));
        zmsg_addstr(msg,tmp.c_str());
      }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
  }
  //_dbg("listar");
  //zmsg_print(msg);
}


/**********************************************************
 * Request
 *********************************************************/

void requestTracker(zctx_t *context, string song, void *tracker){
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"request");
  zmsg_addstr(msg,song.c_str());
  zmsg_send(&msg,tracker);
  zmq_pollitem_t items[] = {{tracker, 0, ZMQ_POLLIN, 0}};
  zmsg_t *incmsg;
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      incmsg = zmsg_recv(tracker);
      break;
    }
  }

  string st;
  int i = -1,parts = atoi(zmsg_popstr(incmsg));
  vector<vector<string> > sng(parts);
  while(zmsg_size(incmsg) > 0){
    st = zmsg_popstr(incmsg);
    if(st == "**") {
      i++;
      continue;
    }
    sng[i].push_back(st);
  }

  //sort(sng.begin(), sng.end(),compare);
  set<string> act;
  bool flag = true;
  while(flag){
    for(int i = 0; i < sng.size(); i++){
      flag = false;
      for(int j = 0; j < sng[i].size(); j++) {
        flag = true;
        if(act.count(sng[i][j]) == 0) {
          string dirNode = sng[i][j];
          sng[i].clear();
          act.insert(dirNode);
          connectNode(context, dirNode,song,i,tracker,act);
        }
      }
    }
  }
  mysongs.push_back(song);
}




/**********************************************************
 * connectNode
 *********************************************************/

void connectNode(zctx_t *context,string dirNode,string song, int part,void *tracker, set<string> &act){
  string dir = "tcp://";
  dir+=dirNode;
  dir+=":5555";
  cout<<dir<<endl;
  void *node = zsocket_new(context,ZMQ_REQ);
  zsocket_connect(node,dir.c_str());
  
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,song.c_str());
  zmsg_addstr(msg,to_string(part).c_str());
  zmsg_print(msg);
  zmsg_send(&msg,node);
  zmsg_t *resp = zmsg_recv(node);

  string name = song.substr(0,song.size()-4);
  if(part < 10) name += "0";
  name += to_string(part);
  name += ".mp3";

  zframe_t* dato;
  zfile_t *download = zfile_new("./canciones/", name.c_str());
  zfile_output(download);
  dato = zmsg_pop(resp);
  
  zchunk_t *chunk = zchunk_new(zframe_data(dato), zframe_size(dato)); 
  zfile_write(download, chunk, 0);
  zfile_close(download);
  zmsg_destroy(&msg);
  zmsg_destroy(&resp);
  zframe_destroy(&dato);

  //nueva parte
  zmsg_t *msgT = zmsg_new();
  zmsg_addstr(msgT,"npart");
  zmsg_addstr(msgT,myip.c_str());
  zmsg_addstr(msgT,song.c_str());
  string tmp = to_string(part);
  zmsg_addstr(msgT,tmp.c_str());
  zmsg_send(&msgT,tracker);

  mt.lock();
  act.erase(dirNode);
  mt.unlock();
}

void retire(void *tracker){
  zmsg_t *msg = zmsg_new();
  mt.lock();
  can = 0;
  mt.unlock();
  zmsg_addstr(msg,"retire");
  zmsg_addstr(msg,myip.c_str());
  for(int i = 0; i < mysongs.size(); i++) zmsg_addstr(msg,mysongs[i].c_str());
  zmsg_send(&msg,tracker);
}


/**********************************************
 *
 *                       UTILIDADES
 *
 **********************************************/


/***********************************************
 * Split mp3
 **********************************************/

int partir(string name){
  string nname;
  char * buffer;
  long size,out,actpart = 0,sizept;
  nname = "canciones/";
  nname += name;
  name = nname;
  ifstream infile (name.c_str(),ifstream::binary);
  name = name.substr(0,name.size()-4);
  // get size of file
  infile.seekg(0,infile.end);
  size=infile.tellg();
  infile.seekg(0,infile.beg);
  out = ceil(size/(double)PARTSIZE);
  //halfSize = static_cast<int>(size - PARTSIZE);
  // allocate memory for file content
  while(size){
    if(PARTSIZE < size) {
      size-= PARTSIZE;
      sizept = PARTSIZE;
    }else{
      sizept = size;
      size = 0;
    }
    buffer = new char[sizept];
    nname = name;
    if(actpart < 10) nname += "0";
    nname += to_string(actpart++);
    nname += ".mp3"; 
    ofstream outfile (nname.c_str(),ofstream::binary);

    // read content of infile
    infile.read (buffer,sizept);
    infile.seekg((sizept*actpart)+1);

    // write to outfile
    outfile.write (buffer,sizept);

    // release dynamically-allocated memory
    delete[] buffer;
    outfile.close();
  }
  infile.close();
  return out;
}

/*bool compare(const vector<string> &a,const vector<string> &b){
  return (a.size() < b.size());
}*/
