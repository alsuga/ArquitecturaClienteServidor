#include <iostream>
#include <thread>

using namespace std;

long out;

long fib(long a){
  if(a < 2) return a;
  return fib(a-1) + fib(a-2);
}

void ffib(long a){
  if( a < 0) a = 0;
  out = fib(a);
}

int main(){
  long tmp,in; cin>>in;
  thread t(ffib,in-2);
  tmp = fib(in-1);
  if(tmp < 0) tmp = 0;
  t.join();
  cout<<out+tmp<<endl;
  return 0;
}



