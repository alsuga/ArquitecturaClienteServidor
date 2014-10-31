#include <bits/stdc++.h>

#define endl '\n'

using namespace std;

void parser(vector<string> &canciones, string &lista){
  size_t pos,ant = 0;
  string act;
  while(true){
    pos = lista.find(";",ant); 
    if(pos == string::npos) break;
    act = lista.substr(ant,pos-ant-4);
    if(!binary_search(canciones.begin(), canciones.end(),act))
      canciones.push_back(act);
    ant = pos+1;
  }
  sort(canciones.begin(), canciones.end());
}

int main(int argc, char** argv) {
  vector<string> canciones;
  string lista = "unacancion.mp3;doscanciones.mp3;trescanciones.mp3;cuatrocanciones.mp3;cincocanciones.mp3;";
  cout<<lista<<endl;
  parser(canciones, lista);
  for(int i = 0; i < canciones.size(); i++) cout<<canciones[i]<<endl;
  return 0;
}
