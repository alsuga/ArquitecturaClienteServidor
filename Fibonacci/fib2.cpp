#include <iostream>
#include <thread>

using namespace std;

void fib(long a, long * out){
  long uno, dos;
  if(a <= 1){
    *out = (a==1)?1:0;
  } else{
    thread t1(fib,a-1,&uno);
    thread t2(fib,a-2,&dos);
    t1.join();
    t2.join();
    *out = uno + dos;
  }
}

int main(){
  long tmp,in; cin>>in;
  fib(in,&tmp);
  cout<<tmp<<endl;
  return 0;
}
