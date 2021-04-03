#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {

    // 4 different test cases
    pid_t testcase[4];

    //testing input
    std::string fileName = "airline";
    std::string passenger[] = {"10","hello","100"};
    std::string baggage[] = {"2", "3", "3"};
    std::string security[] = {"1", "-5", "5"};
    std::string fa[] = {"2", "", "2"};
    // i = 0; 10 2 1 2
    // i = 1; hello 3 -5 2 (input mismatch exception)
    // i = 2; 100 3 5 2
    // i = 3; NULL NULL NULL NULL (logic exception)

    for (int i = 0; i < 4; i ++) {
        testcase[i] = fork();
        if (testcase[i] < 0) {
            std::cout << "Error, no pid create\n"; 
        }
        else if (testcase[i] == 0) {
            if (i == 3) {
                execl(fileName.c_str(), fileName.c_str(), NULL);
            }
            else {
                execl(fileName.c_str(),fileName.c_str(), 
                passenger[i].c_str(), baggage[i].c_str(),security[i].c_str(),
                fa[i].c_str(),NULL);
            }

        }
        else {
            wait(&testcase[i]);
            std::cout<< "----------------------------------------\n";
        }
    }

}