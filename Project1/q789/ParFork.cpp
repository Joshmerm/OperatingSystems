#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int N = 10;

void calculate(int start,int end, vector <string> lines, ofstream &out)
{
    
    string temp;

    string compress;

    string newString;

    int count = 0;

    //Set i = start, and i < end, so I could just read certain lines.
    for (int i = start; i < end;i++)
    {
        temp = lines[i];
        for (int j = 0; j < temp.size();j++)
        {
            if (temp[j] == '0' && temp[j + 1] == '0')
            {
                count++;
                compress += temp[j];
            }
            else if (temp[j] == '1' && temp[j + 1] == '1')
            {
                count++;
                compress += temp[j];
            }
            else
            {
                if (count++ >= 15){
                    string value = to_string(count);
                    temp[i] == '1' ? compress = "+" + value + "+" : compress = "-" + value + "-";
                }
                else
                {
                    compress += temp[j];
                }
                count = 0;
                newString += compress;
                compress = "";
            }
        }
        newString += "\n";
    }
    
    out << newString;
}

int main(int argc,char** argv)
{
    //Creating a input stream and output stream to read and write the file
    ifstream in(argv[1]);
    ofstream out(argv[2]);

    string temp;
    vector <string> lines = {};

    while(getline(in,temp))
    {
        lines.push_back(temp);
    }
    //Ex
    //121 / 10 = 12 for line, 1
    // 0 , 12 , line 0 - 11
    // 12. 24
    // 24 , 36
    int equalLine = lines.size() / 10;
    int remainder = lines.size() % 10;
    int start = 0;
    int end = equalLine;
    pid_t pids[10];

    //Parent process
    //Child process x 10
    for (int i = 0;i < N;i++){
        pids[i] = fork();

        //In my last pid, add the remainder to the end.
        if (pids[i] == 0)
        {
            if (i == 9){
                end += remainder;
            }
            calculate(start,end,lines,out);
            //Prevent child to create another process
            break;
        }
        else{
            //Waiting a child process finish.
            wait(&pids[i]);
        }
        start += equalLine;
        end += equalLine;
    }
    return 0;
}