#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>
#include <ctime>
#include <fstream>

int main(int argc, char* argv[]) {

    std::chrono::duration<double>  between;
    // std::chrono::duration<double>  between2;

    std::string itself = argv[1];
    std::string concatItself = "./" + itself;

    

    pid_t pid1 = fork();

    if (pid1 < 0) {
        std::cout<< "Error, no child process create" << std::endl;
    }
    else if (pid1 == 0) {
        execl(concatItself.c_str(),argv[1],argv[3],argv[4],NULL);    
    }
    else {
        auto start = std::chrono::system_clock::now();
        wait(&pid1);
        between = std::chrono::system_clock::now() - start;
    }


    // std::string itself2 = argv[2];
    // std::string concatItself2 = "./" + itself2;

    // pid_t pid2 = fork();

    // if (pid2 < 0) {
    //     std::cout<< "Error, no child process create" << std::endl;
    // }
    // else if (pid2 == 0) {  
    //     execl(concatItself2.c_str(),argv[2],argv[3],argv[4],NULL);     
    // }
    // else {
    //     auto start2 = std::chrono::system_clock::now();
    //     wait(&pid2);
    //     between2 = std::chrono::system_clock::now() - start2;
    // }


    std::ofstream output(argv[4]);

    output<< "It took: " << between.count() << "s to finish." << std::endl;
    // output<< "It took: " << between.count() << "s to finish ParFork." << std::endl;
    // output<< "It took: " << between2.count() << "s to finish ParThread." << std::endl;


    output.close();







}
