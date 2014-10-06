#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char *mult(string size,string mat,string vec){
  int n,m,o,tmp;
  stringstram ss;
  ss<<size;
  ss>>n>>m>>o;
  int vec[o], out[o];
  memset(out,0,sizeof(out));
  ss<<vec;
  for(int i = 0; i<o;i++) ss>>vec[i];
  ss<<mat;
  int mat[n][m];
  for(int i = 0; i < n; i++)
    for(int j = 0; j < m; j++) ss>>mat[i][j];
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      out[i] += vec[j]*mat[i][j];
    }
  }
  string sout;
  for(int i = 0; i < o; i++){
    sout+= to_string(out[i]);
    sout+= " ";
  }
  return sout.c_str();
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  zmsg_print(in_msg);
  char *nums = zmsg_popstr(in_msg);
  char *mat = zmsg_popstr(in_msg);
  char *vec = zmsg_popstr(in_msg);
  zmsg_addstr(out_msg,"answer");
  zmsg_addstr(out_msg,dir);
  zmsg_append(out_msg, &wh);
  zmsg_addstr(out_msg,mult(st1,st2));
  }
  zframe_destroy(&wh);
}


int main(int argc, char** argv) {
  zctx_t* context = zctx_new();

  void* server = zsocket_new(context, ZMQ_DEALER);

  int c = zsocket_connect(server, "tcp://localhost:5555");
  cout << "connecting to server: " << (c == 0 ? "OK" : "ERROR") << endl;


  zmsg_t *reportmsg= zmsg_new();
  cout<<"Reportandoce!"<<endl;
  zmsg_addstr(reportmsg,"reporting");
  zmsg_send(&reportmsg,server);
  cout<<"Listo"<<endl;

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
      cout<<"Esperando ..."<<endl;
      sleep(1);

      zmsg_send(&outmsg,server);
      cout<<"Enviado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}
