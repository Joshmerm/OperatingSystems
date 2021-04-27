// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#define PORT 8080

using namespace std;
// int sock;

struct args
{

    // char *buffer;
    int socket;
};

void *reverse(void *arg);

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
        pthread_t reqeusts;
        struct args *Args = (struct args *)malloc(sizeof(struct args));
        Args->socket = new_socket;
        pthread_create(&reqeusts, NULL, &reverse, (void *)Args);
        pthread_join(reqeusts, NULL);
    }
    return 0;
}

void *reverse(void *arg)
{
    int socket = ((struct args *)arg)->socket;
    char buffer[1024] = {0};
    int valread = read(socket, buffer, 1024);
    // while (valread > 0)
    // {
        printf("Message From Client: %s\n", buffer);
        string str = buffer;
        string reversed = str;

        int indexForReversed = 0;

        for (int i = str.length(); i > 0; i--)
            reversed[indexForReversed++] = str[i - 1];

        char out[reversed.length()];
        strcpy(out, reversed.c_str());

        printf("Message Sent To Client: %s\n", out);
        printf("-------------------------\n");
        send(socket, out, strlen(out), 0);
        memset(buffer, 0, sizeof(buffer));
        valread = read(socket, buffer, 1024);
        close(socket);

    // }
    pthread_exit(0);
}