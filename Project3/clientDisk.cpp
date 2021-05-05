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


    char * b[1024];
    string str = "";
    char *arr_ptr = &argv;
    for(int i = 0; i < strlen(argv); i++)
        str += argv[i];
    cout <<"str" << str << endl;
    // string str = string(argv);

    send(sock, argv, 1024, 0);
    usleep(2000);
    char buffer[1000000] = {0};
    valread = read( sock , buffer, 1000000);
    printf("%s\n",buffer );
    close(sock);

    return 0;
}