#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>

using namespace std;

// void *compress(void*, void*);

int N = 3;

void *compress(void *);

struct arg_struct
{
    char *input;
    char *out;
};

int main(int argc, char **argv)
{

    pthread_t tid[N];
    string input = "";
    vector<string> results;
    ifstream infile(argv[1]);
    if (infile.is_open())
    {
        while (getline(infile, input))
        {
            results.push_back(input);
        }
    }
    else
        cout << "Opening File Error" << endl;
    
    infile.close();

    


    int size = results.size() / N;
    int remainder = results.size() % N;
    int start = 0;
    int end = size;

    for (int i = 0; i < N; i++)
    {
        int rc;
        input = "";

        if (i == N - 1)
        {
            end += remainder;
        }

        for (int j = start; j < end; j++)
            input += results[j] + '\n';

        start = end;
        end += size;
        struct arg_struct arg;
        char *in = (char *) input.c_str();
        arg.input = in;
        arg.out = argv[2];
        cout <<arg.input << endl;
        if(arg.input != "" && arg.input != "\n"){
        rc = pthread_create(&tid[i], NULL, compress, &arg);
        if (rc)
        {
            cout << "Error return code from pthread_create(t1) is" << rc << endl;
            exit(-1);
        }
        }
    }

    for (int i = 0; i < N + 1; i++)
        pthread_join(tid[i], NULL);

    
    // int* status = (int*)malloc(sizeof(int));
    return 0;
}

void *compress(void *arguments)
{
    struct arg_struct *args = (struct arg_struct *)arguments;

    char *out = (char *)args->out;
    cout << "out" << out << endl;

    char *input = (char *)args->input;
    cout << (args->input) << endl;

    ofstream output(out, ios::app);
    int counter2 = 0;
    int counter = 1;
    for (int i = 0; i < sizeof(input); i++)
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
                    string temp = "+" + to_string(counter) + "+";
                    output << temp;
                }
                else
                {
                    string temp = "-" + to_string(counter) + "-";
                    output << temp;
                }
            }
            counter = 1;
        }
    }
    output.close();
    pthread_exit(NULL);
    // return nullptr;
}
