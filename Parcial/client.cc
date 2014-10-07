#include <bits/stdc++.h>
#include <czmq.h>

#define endl '\n'

using namespace std;

int main(int argc, char** argv) {
  int n,m,o;
  string mat,stmp;
  zctx_t* context = zctx_new();
  void* server = zsocket_new(context,ZMQ_DEALER);

  zsocket_connect(server, "tcp://localhost:4444");
  zmsg_t *msg = zmsg_new();
  freopen("matrices.txt","r",stdin);
  cin>>n>>m>>o;

  mat += to_string(n);
  mat += " ";
  mat += to_string(m);
  mat += " ";
  mat += to_string(o);
  zmsg_addstr(msg,mat.c_str());
  string mat1[n];
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      cin>>stmp;
      if(j) mat1[i] += " ";
      mat1[i] += stmp;
    }

  }
  string mat2;
  for(int i = 0; i < m; i++){
    for(int j = 0; j < o; j++){
      cin>>stmp;
      if(i+j) mat2 += " ";
      mat2 += stmp;
    }
  }
  zmsg_addstr(msg,mat2.c_str());
  for(int i = 0; i < n; i++) zmsg_addstr(msg,mat1[i].c_str());
  zmsg_print(msg);

  zmsg_send(&msg,server);

  zmq_pollitem_t items[] = {{server, 0, ZMQ_POLLIN, 0}};
  string out[n];
  int pos = 0,lg;
  while(pos < n) {
    zmq_poll(items,1,10*ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      zmsg_t *incmsg = zmsg_recv(server);
      lg = atoi(zmsg_popstr(incmsg));
      out[lg] = zmsg_popstr(incmsg);
      pos++;
      zmsg_destroy(&incmsg);
      zmsg_destroy(&msg);
    }
  }
  cout<<"Respuesta"<<endl;
  for(int i =0; i < n; i++) cout<<out[i]<<endl;
  zctx_destroy(&context);
  return 0;
}
