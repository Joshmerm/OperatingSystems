#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
using namespace std;

void process_fork(string, string);

int N = 10;

int main(int argc, char **argv)
{
    string input = "";
    vector <string> results;
    ifstream infile(argv[1]);
    if (infile.is_open()){
        while (getline(infile, input)){
            results.push_back(input);
        }
    }
    else
        cout << "Opening File Error" << endl;

    pid_t pids[N];

    // clear file
    ofstream ofs;
    ofs.open(argv[2], ofstream::out | ofstream::trunc);
    ofs.close();

    int size = results.size() / N;
    int remainder = results.size() % N;
    int start = 0;
    int end = size;

    for (int i = 0; i < N; i++){

        input = "";
        if(i == N - 1){
            end += remainder;
        }

        for(int j = start; j < end; j++)
            input += j < results.size() ? results[j] + '\n' : "";

        start = end;
        end += size;


        pids[i] = fork();

        if (pids[i] == 0)
            process_fork(input, argv[2]);
        else if(pids[i] > 0){
            wait(&pids[i]);
        }
    }
    int status;
    pid_t pid;
    int n = N;
    while (n > 0)
    {
        pid = wait(&status);
        n--;
    }
    infile.close();

    return 0;
}

void process_fork(string input, string out)
{
    ofstream output(out, ios::app);
    int counter2 = 0;
    int counter = 1;
    for (int i = 0; i < input.size(); i++)
    {
        if (input[i + 1] - input[i] == 0)
        {
            counter++;
        }
        else
        {
            counter2 += counter;
            if (counter < 16)
            {
                for (int i = counter2 - counter; i < counter2; i++)
                {
                    output << input[i];
                }
            }
            else
            {
                if (input[counter2 - 1] == '1')
                {
                    output << "+" << counter << "+";
                }
                else
                {
                    output << "-" << counter << "-";
                }
            }
            counter = 1;
        }
    }
    output.close();
    exit(0);
}
