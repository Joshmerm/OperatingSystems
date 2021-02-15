
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
 #include <sstream>
using namespace std;

int main(int argc, char** argv){
  pid_t pid;
  string command;
  string temp;

  while(command.compare("exit") != 0){
    cout<<"MoreShell>";
    getline(cin, command); 

    stringstream cmdss = (stringstream) command;
    vector<string> list = {};

    while (getline(cmdss,temp,' '))
        list.push_back(temp);

    char * listFinal[list.size() + 1];

    for(int i = 0; i < list.size(); i++)
        listFinal[i] = (char*) list[i].c_str();
    
    listFinal[list.size()] = NULL;

    pid = fork();
    if(pid < 0 )
        printf("Failed to create the pipe");
    else if(pid == 0){
        execvp(listFinal[0], listFinal);
    }
    else{
        wait(&pid);
    }
    fflush(stdout);
        
  }
  exit(0);
}
