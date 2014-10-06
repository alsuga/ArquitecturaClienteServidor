//  Hello World server

#include <czmq.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cassert>

using namespace std;

typedef unordered_map<string,string> DictType;

// Server's state
DictType dict;

void dispatch(zmsg_t *incmsg, zmsg_t *outmsg) {
  zmsg_print(incmsg);
  char *operation = zmsg_popstr(incmsg);
  
  if(strcmp(operation,"Create") == 0) {
    char *wordToLookUp = zmsg_popstr(incmsg);
    string w(wordToLookUp); 
    free(wordToLookUp);
    if (dict.count(w) > 0) {
      zmsg_addstr(outmsg, dict[w].c_str());
    } else {
      zmsg_addstr(outmsg, "Your account number is:");
    }
    
  } else if (strcmp(operation,"Consult") == 0) {
    char *wordToAdd = zmsg_popstr(incmsg);
    char *meaningToAdd = zmsg_popstr(incmsg);   
    string w(wordToAdd), m(meaningToAdd);
  free(wordToAdd);
  free(meaningToAdd);
    dict[w] = m;
    zmsg_addstr(outmsg, "Added");
  } else if (strcmp(operation,"Deposit") == 0) {
    char *wordToAdd = zmsg_popstr(incmsg);
    char *meaningToAdd = zmsg_popstr(incmsg);   
    string w(wordToAdd), m(meaningToAdd);
  free(wordToAdd);
  free(meaningToAdd);
    dict[w] = m;
    zmsg_addstr(outmsg, "Added");
  } else if (strcmp(operation,"Transfer") == 0) {
    char *wordToAdd = zmsg_popstr(incmsg);
    char *meaningToAdd = zmsg_popstr(incmsg);   
    string w(wordToAdd), m(meaningToAdd);
  free(wordToAdd);
  free(meaningToAdd);
    dict[w] = m;
    zmsg_addstr(outmsg, "Added");
  }  else {
    zmsg_addstr(outmsg, "Error!!");
  }
}


int main (void)
{
  zctx_t *context = zctx_new();
  void *responder = zsocket_new(context, ZMQ_REP);
  zsocket_bind(responder, "tcp://*:5555");
  
  while (1) {
    zmsg_t* request = zmsg_recv(responder);
    zmsg_t *response = zmsg_new();
    dispatch(request,response);
    zmsg_send(&response,responder);
    }
    
    zctx_destroy(&context);
    return 0;
}




