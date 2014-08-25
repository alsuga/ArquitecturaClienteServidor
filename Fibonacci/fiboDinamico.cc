#include <bits/stdc++.h>
#include <thread>
#include <mutex>


using namespace std;

unordered_map<long,long> memo;
mutex mt;

void fib(long n, long *out){
  if(n == 1 || n == 2) {
    *out = 1 ;
    return;
  }

  if(memo[n] != 0) {
    *out = memo[n];
    return;
  }
  long fp,fpp;  
  thread t(fib,n-1,&fp);
  thread t1(fib,n-2,&fpp) ;
  t.join();
  t1.join();
  mt.lock();
  *out = fp + fpp;
  memo[n]=*out; 
  mt.unlock();
}


int main(){
  long out,in;
  for(in = 2; in < 50; in++){
    fib(in,&out);
    cout<< out << endl;
  }
  return 0;
}
