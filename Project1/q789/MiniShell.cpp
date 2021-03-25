#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <vector>
#include <cstring>

using namespace std;

int main(int argc,char** argv){

    //Create a char* pointer variable, and named it command.
    char* command;

    pid_t pid;

    //While loop to check whenever the command is not equal to exit.
    while (strcmp(command,"exit") != 0){
        
        cout << "# ";

        //Allow user to type in the command.
        cin.getline(command,50);

        pid = fork();

        if (pid <0){
            printf("Error, fork failed");
        }
        else if (pid == 0){
            //Replaces the current process with a new process specfifed by command.
            execlp(command,command,NULL);
        }
        else{
            wait(&pid);
        }
    }

    return 0;
}