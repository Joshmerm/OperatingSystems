#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

void child(char ** argv){
    execl(argv[1], argv[1], argv[2], argv[3], NULL);
}

void parent(){
    cout << "Child Is Done" << endl;
}

int main(int argc, char** argv) { 
    pid_t pid;
    pid = fork();

    if(pid < 0) {
        perror("Error");
        exit(1);
    }else if(pid == 0){
        child(argv);
        exit(1);
    }else{
        wait(&pid);
        parent();
    }

    return 0; 
} 