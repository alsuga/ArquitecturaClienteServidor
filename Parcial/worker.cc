#include <bits/stdc++.h>
#include <czmq.h>
#define c(x) cout<<x<<endl;


using namespace std;

string mult(string size,string mat,string vec){
  int n,m,o,tmp;
  stringstream ss;
  ss<<size<<' '<<vec<<' '<<mat;
  ss>>n>>m>>o;
  int vect[o], out[o];
  memset(out,0,sizeof(out));
  for(int i = 0; i<o;i++) 
    ss>>vect[i];
  int matriz[n][m];

  for(int i = 0; i < n; i++)
    for(int j = 0; j < m; j++) ss>>matriz[i][j];
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      out[i] += vect[j]*matriz[j][i];
    }
  }
  string sout;
  for(int i = 0; i < o; i++){
    if(i)sout+= " ";
    sout+= to_string(out[i]);
  }
  return sout;
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  cout<<"despachando..."<<endl;
  char *pos = zmsg_popstr(in_msg);
  zframe_t *wh = zmsg_pop(in_msg);
  char *dir = zmsg_popstr(in_msg);
  char *nums = zmsg_popstr(in_msg);
  char *mat = zmsg_popstr(in_msg);
  char *vec = zmsg_popstr(in_msg);
  zmsg_addstr(out_msg,"answer");
  zmsg_addstr(out_msg,pos);
  zmsg_append(out_msg, &wh);
  zmsg_addstr(out_msg,dir);
  zmsg_addstr(out_msg,mult(nums,mat,vec).c_str());
  zframe_destroy(&wh);
  cout<<"despachado"<<endl;
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
      cout<<"Esperando..."<<endl;
      sleep(1);
      zmsg_send(&outmsg,server);
      cout<<"Enviado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}
