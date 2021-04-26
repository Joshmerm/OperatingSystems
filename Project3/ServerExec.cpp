// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>

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
        char buffer;
        int received_int = 0;
        int return_status = read(new_socket, &received_int, sizeof(received_int));
        int first = 1;
        int index = 0;
        char *commands[ntohl(received_int)];
        string s = "";
        while (return_status > 0)
        {

            if (first == 1)
            {
                fprintf(stdout, "Received int = %d\n", ntohl(received_int));
                first = 0;
            }
            else
            {
                s += buffer;
                s += " ";
                commands[index++] = &buffer;
                cout << &buffer << endl;
            }
            return_status = read(new_socket, &buffer, sizeof(char *));
        }
        commands[index++] = NULL;

        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {

            // int fds = open("t.txt", O_CREAT | O_TRUNC | O_RDWR, 0644);
            // if (fds < 0)
            // {
            //     perror("open()");
            //     exit(EXIT_FAILURE);
            // }
            // int stdout_copy = dup(STDOUT_FILENO);
            close(STDOUT_FILENO);
            // dup2(fds, STDOUT_FILENO);
            dup2(new_socket, STDOUT_FILENO);
            // dup2(new_socket, STDERR_FILENO);
            if (execvp("ls", commands) < 0)
            {
                fprintf(stderr, "Could not execute");
                exit(1);
            }else{
                fprintf(stderr, "execute");
            }

            // dup2(stdout_copy, STDOUT_FILENO);
            // close(stdout_copy);
            // char output[10000];
            // ifstream myReadFile;
            // myReadFile.open("t.txt");

            // while (!myReadFile.eof())
            // {
            //     myReadFile >> output;
            //     cout << output;
            // }
            // myReadFile.close();
        }
        else
        {
            // close(new_socket);
            wait(&pid);
        }
    }
    return 0;
}
