#include <bits/stdc++.h>
#include <dirent.h>

using namespace std;

map<string,int> uso;

void reproducida(string cancion){
  map[cancion]++;
  if(map[cancion] > 10){
    map[cancion] = 0;
    replicar(cancion);
  }
}

int listar(){
  string canciones = "";
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ("canciones")) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      if( strcmp(ent->d_name,".") != 0 and strcmp(ent->d_name, "..") != 0){
        canciones += ent->d_name;
        canciones += ';'; 
      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    return EXIT_FAILURE;
  }
  cout<<canciones<<endl;
}

int main(){
  
  return 0;
}
