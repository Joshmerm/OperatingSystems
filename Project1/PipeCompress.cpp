#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>

using namespace std;

string compress (string line);

int main(int argc, char** argv){

    ifstream in(argv[1]);
    ofstream out(argv[2]);
    pid_t pid;
    int p[2];
    char wr_msg[1] = "";
    char rc_msg[1] = "";

    if(pipe(p) == -1){
        perror("Error Creating The Pipe");
        exit(0);
    }

    pid = fork();
    //child
    if(pid == 0){
        string line;
        close(p[0]);
        while (getline (in, line)){
            string lineCompressed = compress(line);
            for(int i = 0; i < lineCompressed.size(); i++){
                wr_msg[0] = lineCompressed[i];
                write(p[1], wr_msg, 1);
            }
            write(p[1], "\n", 1);
        }
        close(p[1]);
        wait(&pid);
    //parent
    }else{
        close(p[1]);
        while(read(p[0], rc_msg, 1) > 0) out << rc_msg;
        close(p[0]);
    }

    return 0;
}

string compress (string line){
    string temp = "";
    string returnString;
    int count = 0;
    
    for(int i = 0; i < line.size(); i++){
        if((line[i] - line[i + 1]) == 0){
            count++;
            temp += line[i];
        }else{
            if(count++ >= 15){
                if(line[i] == '1') temp = "+" + to_string(count) + "+";
                else temp = "-" + to_string(count) + "-";
            } else {
                temp += line[i];
            } 
                
            returnString += temp;
            count = 0;
            temp = "";
        }
        // returnString += '\n';
    }
    // out << returnString;
    return returnString;
}