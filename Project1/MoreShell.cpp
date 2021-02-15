
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
// #include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

int main(int argc, char** argv){
  pid_t pid;
  string command;
  string temp;

  while(command.compare("exit") != 0){
    cout<<"MoreShell>";
    command = "";
    getline(cin, command); 
    vector<string> list;
    char * listFinal[50];

    for(auto x : command){
        if(isspace(x)) {
            list.push_back(temp);
            temp = "";
        }
        else temp += x;
    }
    list.push_back(temp);

    for(int i = 0; i < list.size(); i++)
        listFinal[i] = (char*) list[i].c_str();

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
