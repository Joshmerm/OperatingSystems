#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <stdio.h>

int main() {
    std::string command;

    while (command.compare("exit") != 0) {
       std::cout << "DupShell>";

        //take a line of command
        //cannot deal with wrong input, it has to type extra exit in order to exit
        getline(std::cin,command);

        std::vector<std::string> input;


        std::stringstream check(command);
        std::string temp;
        
        int counter = 0; // count how many element before |
        int stopCount = 0; // when counter reaches |, even counter still count but ignore it
        
        //separate the command into parts to the vector and count when does it reach to |
        while (getline(check, temp, ' ')) {
            input.push_back(temp);
            if (temp.compare("|") == 0) {
                stopCount = counter;
            }
            counter ++;
        }

        if (stopCount == 0) {
            stopCount = counter;
        }
        
        //create two lists to store the separated command and execute in order
        //command/argument before |
        //stopCount + 1 becasue 1 extra for NULL
        char* list[stopCount + 1];

        for (int i = 0; i < stopCount; i ++) {
            list[i] = (char*) input[i].c_str();
        }
    
        list[stopCount] = NULL;

        //command/argument after |
        //input size - stopCount already gives a extra space for NULL since we don't want |
        int subargumentSize = (input.size() - stopCount);
        char* list2[subargumentSize];

        //start from 0 for the new list, continue from i for the input vector
        for (int i = stopCount + 1, j = 0; i < input.size(); i ++, j++) {
            list2[j] = (char*) input[i].c_str();
        }

        list2[subargumentSize - 1] = NULL;

        //pipe to store temporary stdout 
        int p[2];

        if (pipe(p) == - 1) {
            std::cout<< "Error, no pipe create" << std::endl;
        }
        
        pid_t pid = fork();



        if (pid < 0) {
            std::cout << "Error, no PID create" << std::endl;
        }
        else if (pid == 0) { 
            //execute the first part of the command
            dup2(p[1],STDOUT_FILENO);
            close(p[1]);
            close(p[0]);
            execvp(list[0],list);
        }
        else {
            //wait for the first part of the command finish
            //then create another process to run the command after |
            wait(&pid);
            pid_t pid2 = fork();

            if (pid2 < 0) {
                std::cout << "Error, no PID create" << std::endl;
            }
            //close both end of the pipe to avoid command after | ask for more input and output
            if (pid2 == 0) {
                dup2(p[0],STDIN_FILENO);
                close(p[1]);
                close(p[0]);
                execvp(list2[0],list2);
                }
            else {
                close(p[1]);
                close(p[0]);
                wait(&pid2);
                
                
            }
        }
    }

    return 0;

}