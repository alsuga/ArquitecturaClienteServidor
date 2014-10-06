#include <bits/stdc++.h>
#include <czmq.h>

using namespace std;

int main(int argc, char** argv) {
 
  zctx_t *context = zctx_new();
  void *requester = zsocket_new(context, ZMQ_REQ);
  zsocket_connect(requester, "tcp://localhost:5555");

  // Creates an empty message
  zmsg_t* request = zmsg_new();
  
  if (strcmp(argv[1],"create") == 0) {
    zmsg_addstr(request,"Create");
    zmsg_addstr(request,argv[2]);
  } else if (strcmp(argv[1],"consult") == 0) {
    zmsg_addstr(request,"Consult");
    zmsg_addstr(request,argv[2]);
  } else if (strcmp(argv[1],"deposit") == 0) {
    zmsg_addstr(request,"Deposit");
    zmsg_addstr(request,argv[2]);
    zmsg_addstr(request,argv[3]);
  } else if (strcmp(argv[1],"retire") == 0) {
    zmsg_addstr(request,"Retire");
    zmsg_addstr(request,argv[2]);
    zmsg_addstr(request,argv[3]);
  } else if (strcmp(argv[1],"transfer") == 0) {
    zmsg_addstr(request,"Transfer");
    zmsg_addstr(request,argv[2]);
    zmsg_addstr(request,argv[3]);
    zmsg_addstr(request,argv[4]);
  } else {
    cout << "error!\n";
  } 
  
  // Sends message request through socket requester
  zmsg_send(&request,requester);
  
  zmsg_t* resp = zmsg_recv(requester);
  
  zmsg_print(resp);      
         
  zctx_destroy(&context);
 
  return 0;
}


void crearCuenta(){
  puts("Ingrese numero de identificacion:");
  cin>>num;
  // Server crear responde numero de cuenta
  puts("Guarde su numero de cuenta, lo necesitara en el futuro, este es:");
  cout<<cuenta<<endl;
}

void deposito(){
  puts("Ingrese su numero de cuenta y la cantidad a depositar:");
  cin>>num>>dep;
  //server responde
  if(flag == 0) puts("Error");
}