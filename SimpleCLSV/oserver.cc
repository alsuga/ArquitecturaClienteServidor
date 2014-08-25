#include <zmq.h>
#include <cstdio>
#include <unistd.h>
#include <string.h>
#include <assert.h>

using namespace std;

void solve(char *buffer){
  int x,y,out;
  char op;
  sscanf(buffer,"%i %c %i",&x,&op,&y);
  switch(op){
    case '+':
      out = x + y;
      break;
   
    case '-':
      out = x - y;
      break;
  
    case '*':
      out = x * y;
      break;
  
    case '/':
      if(y!=0) out = x / y;
      else strcpy(buffer,"Division by zero\0");
      break;
  }
  if(buffer[0] != 'D') sprintf(buffer,"%i\n\0",out);
  
}

int main (void)
{
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

    while (1) {
        char buffer [20];
        zmq_recv (responder, buffer, 20, 0);
        printf ("Received operation \"%s\"\n",buffer);
        solve(buffer);
        zmq_send (responder, buffer, 20, 0);
    }
    return 0;
}
