#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

void child(char ** argv){
    //Runs MyCompress with the two command line arguments
    execl("MyCompress", "MyCompress", argv[1], argv[2], NULL);
}

void parent(){
    //Parent function, runs once child is done
    cout << "Child Is Done" << endl;
}

//Takes in two command Line arguments (input file path, output path)
// Example for running: ./ForkCompress source.txt destination.txt 
int main(int argc, char** argv) { 
    pid_t pid;
    //Forks the process, parent will wait until child executes
    pid = fork();

    if(pid < 0) {
        //Error Forking
        perror("Error");
        exit(1);
    }else if(pid == 0){
        //Executes the child function
        child(argv);
        exit(1);
    }else{
        //Waits for child to finish
        wait(&pid);
        //Once child is done, executes the Parent Function
        parent();
    }

    return 0; 
} 