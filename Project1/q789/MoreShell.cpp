#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>

int main() {
    std::string command;

    while (command.compare("exit") != 0) {
        std::cout << "MoreShell>";

        //take a line of command
        //cannot deal with wrong input, it has to type extra exit in order to exit
        getline(std::cin,command);


        //separate the command into parts to the vector
        std::vector<std::string> input;


        std::stringstream check(command);
        std::string temp;

        while (getline(check, temp, ' ')) {
            input.push_back(temp); 
        }

        //create a list to store the separated commend and execute
        char* list[input.size() + 1];

        for (int i = 0; i < input.size(); i ++) {
            list[i] = (char*) input[i].c_str();
        }

        // null at the end of the list of argument
        list[input.size()] = NULL;

        pid_t pid = fork();

        if (pid < 0) {
            std::cout << "Error, no PID create" << std::endl;
        }
        else if (pid == 0) {
            //use the list to run execvp
            execvp(list[0],list);
        }
        else {
            wait(&pid);
        }

        
    }

    return 0;

}