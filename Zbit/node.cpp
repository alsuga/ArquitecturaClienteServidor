#include <bits/stdc++.h>
#include <czmq.h>
#include <dirent.h>


#define PARTSIZE 524288

using namespace std;

/***********************
 * Variables Globales
 **********************/
string myip;
vector<string> mysongs;
int can = 1;


/***********************
 * Funciones
 **********************/
void parser(vector<string> &canciones, string &lista);
void connectNode(void *node,string dirNode);
void report(zmsg_t *msg);
int partir(string name);

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

  //MENU
  thread t(listening,listen);

  zctx_destroy(&context);
  return 0;
}


/**************************************
* COMPARTIENDO CANCIONES
**************************************/

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
  while(can){
    zmsg_t* request = zmsg_recv(listen);
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
        zmsg_addstr(msg,to_string(partir(ent->d_name)).c_str());
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

void requestTracker(string song,void *tracker){
  zmsg_t *msg = zmsg_new();
  zmsg_addstr(msg,"request");
  zmsg_addstr(msg,song.c_str());
  zmsg_send(&msg,tracker);
  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  zmsg_t *incmsg;
  while(true) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      incmsg = zmsg_recv(server);
      zmsg_print(incmsg);
      break;
    }
  }
  string strout = zmsg_popstr(incmsg),st;
  int i = -1,parts = atoi(zmsg_popstr(incmsg));
  vector<vector<string> > sng(parts);
  while(zmsg_size(msg) > 0){
    st = zmsg_popstr(msg);
    if(st == "**") {
      i++;
      continue;
    }
    parts[i].push_back(st);
  }
  sort(sng.begin(), sng.end(),compare);

}




/**********************************************************
 * connectNode
 *********************************************************/

void connectNode(zctx_t *context,string dirNode,string song, int part,void *tracker){
  void *node = zsocket_new(context,ZMQ_REQ);
  zsocket_connect(node,dirNode.c_str());
  
  zmsg_t *msg = zmsg_new();


  zmsg_t *msgT = zmsg_new();
  zmsg_addstr(msgT,"npart");
  zmsg_addstr(msgT,myip.c_str());
  zmsg_addstr(msgT,song.c_str());
  zmsg_addstr(msgT,to_string(part).c_str());
  zmsg_send(&msgT,tracker);
}

void retire(void *tracker){
  zmsg_t *msg = zmsg_new();
  can = 0;
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

bool compare(vector<string> &a,vector<string> &b){
  return (a.size() < b.size());
}