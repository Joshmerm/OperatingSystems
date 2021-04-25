// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
// #include <iostream>
#include <sstream>
// #include <pthread.h>
#define PORT 8080

using namespace std;

struct args
{
    char *buffer;
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
        char *buffer[1024];
        int received_int = 0;
        int return_status = read(new_socket, &received_int, sizeof(received_int));
        int first = 1;
        int index = 0;
        // int recieved = 0;
        char commands[ntohl(received_int)];
        while (return_status > 0)
        {

            if(first == 1){
                fprintf(stdout, "Received int = %d\n", ntohl(received_int));
                first = 0;
            }else{

                printf("%s\n", buffer);
                // char s( buffer)];
                // s = buffer;
                // commands[index++] = buffer.c_str();
            }
            return_status = read(new_socket, buffer, 1024);

        }

        // for(int i =0; i< ntohl(received_int); i++){
        //     printf("%d: %s", i, commands[i]);
        // }
        // char* buffer[received_int];
        // struct args *Message = (struct args *)malloc(sizeof(struct args));
        // valread = read(new_socket, buffer, received_int*sizeof(char *));
        // Message->buffer = buffer;
        // Message->socket = new_socket;
        // cout << buffer << endl;
        // printf(buffer);

        // pid_t pid = fork();

        // if (pid == -1)
        // {
        //     perror("fork");
        //     exit(EXIT_FAILURE);
        // }
        // else if (pid > 0)
        // {
        //     exec(Message);

        //     wait(nullptr);
        // }
        // else
        // {
        //     wait(&pid);
        //     exit(EXIT_SUCCESS);
        // }
    }
    return 0;
}

void exec(void *arg)
{

    char *buffer = ((struct args *)arg)->buffer;
    int socket = ((struct args *)arg)->socket;

    stringstream geek(buffer);
    int x = 0;
    geek >> x;
    // printf("Message From Client: %d\n", x);
    // char *arr[] = {"ls", "-l"};
    // for (int i = 0; i < 2; i++)
    // {
    //     printf("%s\n", arr[i]);
    // }

    // string str = buffer;
    // string reversed = str;

    // int indexForReversed = 0;

    // for (int i = str.length(); i > 0; i--)
    //     reversed[indexForReversed++] = str[i - 1];

    // char out[reversed.length()];
    // strcpy(out, reversed.c_str());

    // send(socket, out, strlen(out), 0);
    pthread_exit(0);
}