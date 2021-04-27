#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
using namespace std;
int main(int argc, char const *argv[])
{

    // if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    // {
    //     printf("\nConnection Failed \n");
    //     return -1;
    // }

    bool stop = false;
    while (!stop)
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
        // close(sock);
        string input = "";
        char buffer[1024] = {0};
        cout << "Please enter a string ";
        getline(cin, input);
        strcpy(buffer, input.c_str());
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }

        send(sock, buffer, strlen(buffer), 0);
        printf("Message sent To Server: %s\n", buffer);
        valread = read(sock, buffer, 1024);
        printf("From Server: %s\n", buffer);
        close(sock);
    }
    return 0;
}