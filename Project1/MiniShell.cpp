#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
using namespace std;


/*The purpose of this program is to created a minishell that perform some very
    basic commands that shell do.*/
int main(int argc, char** argv){
  pid_t pid;
  string command;
  while(command.compare("exit") != 0){
    cout<<"MiniShell>";
    cin >> command;
    pid = fork();
    if(pid < 0 )perror("Failed to create the pipe");
    else if(pid == 0)
        execlp(command.c_str(), command.c_str(), NULL);
    else wait(&pid);
    fflush(stdout);
  }
  exit(0);
}
