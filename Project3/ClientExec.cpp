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
    // usleep(100);

    for (int i = 0; i < argc; i++)
    {
        int n = write(sock, argv[i], sizeof(char *));
        if (n < 0)
            perror("error Writing to socket");
    }
    char t[] = "hello";
    // cout << "asdasd";
    cout << t << endl;
    char buffer[1000000] = {0};
    usleep(2000);
    valread = read( sock , buffer, 1000000);
    printf("%s\n",buffer );
    // int return_status = read(sock, buffer, strlen(buffer));

    // printf("From Server: %s\n", &buffer);
    // while (return_status > 0)
    // {
    //     cout << buffer << endl;
    //     return_status = read(sock, buffer, 1024);
    //     // cout << "Return Status:" << return_status << endl;
    // }
    close(sock);
    // exit(0);
    // // // valread = read(sock, buffer, 1024);
    // cout << buffer << endl;
    // string s = "Do you see this message\n"
    // return_status = read(sock, buffer, s.length());
    // cout << buffer << endl;
    // close(sock);
    //  cout << "hello" << endl;
    //  return_status = read(sock, buffer, sizeof(char *));
    // printf("From Server: %s\n", buffer);

    return 0;
}