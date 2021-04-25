#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
// #include <arpa/inet.h>

#define PORT 8080
using namespace std;
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int n = argc;
    int tmp = htonl(n);
    write(sock, &tmp, sizeof(tmp));
    usleep(100);
    for(int i = 0; i < argc; i++){
        write(sock, argv[i], 1024);
    }
    // write(sock, &tmp, sizeof(tmp));
    // send(sock, &argv, 0);
    // write(sock, &argc, sizeof(argc));
    // printf("Message sent To Server: ");
    // for(int i = 1; i < argc; i++)
    //     printf("%s, ", argv[i]);
    // valread = read(sock, buffer, 1024);
    // printf("\nFrom Server: %s\n", buffer);
    return 0;
}