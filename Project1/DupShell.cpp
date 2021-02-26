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
  int pipefds[2];
  string command;
  string temp;

  while(command.compare("exit") != 0){
    cout<<"MoreShell>";
    getline(cin, command); 

    stringstream cmd1 = (stringstream) command.substr(0, command.find("|"));
    stringstream cmd2 = (stringstream) command.substr(command.find("|") + 2, command.length() - 1);
    vector<string> list1 = {};
    vector<string> list2 = {};

    while (getline(cmd1,temp,' '))
        list1.push_back(temp);

    while (getline(cmd2,temp,' '))
        list2.push_back(temp);

    char * listFinal1[list1.size() + 1];
    char * listFinal2[list2.size() + 1];

    for(int i = 0; i < list1.size(); i++)
        listFinal1[i] = (char*) list1[i].c_str();
    
    for(int i = 0; i < list2.size(); i++)
        listFinal2[i] = (char*) list2[i].c_str();
    
    listFinal1[list1.size()] = NULL;
    listFinal2[list2.size()] = NULL;


    if(pipe(pipefds) == -1){    
      perror("pipe");    
      // exit(EXIT_FAILURE);  
    }

    pid = fork();
    if(pid == -1 ){
      perror("Fork");
      exit(EXIT_FAILURE);
    } else if(pid == 0){
      dup2(pipefds[1], STDOUT_FILENO);
      close(pipefds[0]);      
      execvp(listFinal1[0], listFinal1);
      // exit(EXIT_SUCCESS);
    }
    else{
      dup2(pipefds[0],STDIN_FILENO); 
      close(pipefds[1]);
      // wait(&pid);
      execvp(listFinal2[0], listFinal2);
      // exit(EXIT_SUCCESS);   
        
    }

    fflush(stdout);
        
  }
  exit(0);
}