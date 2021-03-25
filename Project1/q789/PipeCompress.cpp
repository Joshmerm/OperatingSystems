#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/wait.h>

using namespace std;

string compress(ifstream &in){

    //Create a string varaible,name it line, to help keep track every line in the txt file
    string line;

    //compress, more like a temp, to allow the program add and store letter
    string compress;

    //The variable use for output.
    string newString;

    //To count if a letter repeat more than or equal to 16 times.
    int count = 0;

    while (getline(in,line))
    {
        for (int i = 0;i < line.size();i++)
        {
            if (line[i] == '0' && line[i + 1] == '0')
            {
                count++;
                compress += line[i];
            }
            else if (line[i] == '1' && line[i + 1] == '1')
            {
                count++;
                compress += line[i];
            }
            else
            {
                //Once count reach to 16 or higher 
                if (count++ >= 15){
                    string value = to_string(count);
                    //Using conditional operator to determine the count is 0 or 1
                    line[i] == '1' ? compress = "+" + value + "+" : compress = "-" + value + "-";
                }
                else
                {
                    compress += line[i];
                }
                //Reset count,add newString, and reset compress.
                count = 0;
                newString += compress;
                compress = "";
            }
        }
        newString += "\n";
    }

    //Return the whole string.
    return newString;
    
}

int main(int argc, char** argv)
{
    //Creating a input and output stream to read and write the file.
    ifstream in(argv[1]);
    ofstream out(argv[2]);

    int pi[2];

    pid_t pid;

    if (pipe(pi) < 0){
        printf("Error: fork failed. \n");
    }

    //Use fork to create a child process.
    pid = fork();

    
    if (pid == 0){
        //Using while loop to receive one letter per time and add it to s.
        close(pi[1]);
        string s;
        char inbuf[1];
        while (read(pi[0],inbuf,1) > 0){
            s += inbuf;
        }
        out << s;
        close(pi[0]);
    }
    else{
        //Using a for loop to send one letter per time.
        close(pi[0]);
        string msg = compress(in);
        for (int i = 0;i < msg.size();i++)
        {
            write(pi[1],&msg[i],1);
        }
        close(pi[1]);
        wait(&pid);
    }

    return 0;
}