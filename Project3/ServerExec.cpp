// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <stdio.h>
#include <vector>

// #include <iostream>
#include <sstream>
// #include <pthread.h>
#define PORT 8080

using namespace std;

struct args
{
    char *buffer[10];
    int socket;
};

void exec(void *arg);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        int received_int = 0;
        int return_status = read(new_socket, &received_int, sizeof(received_int));
        int first = 1;
        int index = 0;
        // char * commands[ntohl(received_int)] = {0};
        int firstargv = 1;
        string s ="";
        vector<char *> commandVector;
        commandVector.push_back("ls");
        char *buffer;
        while (return_status > 0)
        {

            if (first == 1)
            {
                fprintf(stdout, "Received int = %d\n", ntohl(received_int));
                first = 0;
            }else if(firstargv == 1){
                firstargv = 0;
                s = "ls";
                // commands[index++] = "ls";
            }
            else
            {
                cout <<"asd" << buffer << endl;
            }
            return_status = read(new_socket, buffer, sizeof(char *));
        }
        commandVector.push_back(NULL);
        char **command = &commandVector[0];
        if(command[0] == NULL)
         cout << "NULL" << endl;
         else cout<< "Not NUll"<<endl;



        // commands[index] = NULL;
        for (int i = 0; i < 3; i++)
        {
            fprintf(stdout,"%s\n", command[i]);
        }

        // close(new_socket);
        int fd = open("t.txt", O_CREAT | O_TRUNC | O_RDWR, 0644);
        if (fd < 0)
        {
            perror("open()");
            exit(EXIT_FAILURE);
        }

        int pid;
        pid = fork();
        if (pid < 0)
            perror("Error Forking");
        if (pid == 0)
        {
            // cout << "asdasdasdasd" << commands[0] << endl;
            close(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            char * s[] = {"ls", NULL};
            execvp(command[0], command);
        }
        else if (pid > 0)
        {
            wait(&pid);
            close(fd);
            int pid2;
            string myText;

            pid2 = fork();
            if (pid2 < 0)
                perror("Error Forking");
            if (pid2 == 0)
            {
                ifstream MyReadFile("t.txt");
                string output = "";
                while (getline(MyReadFile, myText))
                {
                    output += myText + "\n";
                    // cout << myText << endl;
                }
                MyReadFile.close();
                char out[1000000];
                strcpy(out, output.c_str());
                send(new_socket, out, 1000000, 0);
                exit(0);
            }
            else
            {
                
                wait(&pid2);
                close(new_socket);

            }
            // char out[1000000];
            // strcpy(out, output.c_str());
            // char t[] = "hello";
            // send(new_socket, t, strlen(t), 0);
            // write(new_socket, t, strlen(t));
            // close(new_socket);
            // exit(0);
        }
        // fflush(stdout);
    }
    return 0;
}
