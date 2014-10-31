#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

char type;

char *itoa(int in){
  char *out= new char;
  sprintf(out,"%i",in);
  return out;
}

void dispatcher(zmsg_t *in_msg, zmsg_t *out_msg ){
  zframe_t *wh = zmsg_pop(in_msg);
  zmsg_print(in_msg);
  int a = atoi(zmsg_popstr(in_msg)),b = atoi(zmsg_popstr(in_msg));
  char *dir = zmsg_popstr(in_msg);
  zmsg_addstr(out_msg,"answer");
  zmsg_addstr(out_msg,dir);
  zmsg_append(out_msg, &wh);

  if(type == '+') zmsg_addstr(out_msg,itoa(a+b));
  if(type == '-') zmsg_addstr(out_msg,itoa(a-b));
  if(type == '*') zmsg_addstr(out_msg,itoa(a*b));
  if(type == '/'){
    if(b == 0) zmsg_addstr(out_msg,"division por cero");
    else zmsg_addstr(out_msg,itoa(a/b));
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
  zmsg_addstr(reportmsg,argv[1]);
  sscanf(argv[1],"%c",&type);
  zmsg_print(reportmsg);
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
      sleep(5);

      zmsg_send(&outmsg,server);
      cout<<"Enviado"<<endl;
    }
  }
  zctx_destroy(&context);
  return 0;
}
