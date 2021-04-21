// Client side C/C++ program to demonstrate Socket programming
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <cstring>

#define PORT 8080
using namespace std;
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    // char *hello = "Hello from client";
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    // while(true){
        char buffer[1024] = {0};
        char in[1024];
        cout << "Input To Server: ";
        cin.getline(in ,sizeof(in));
        
        send(sock , in , strlen(in) , 0 );
        printf("Message sent To Server: %s\n", in);
        valread = read( sock , buffer, 1024);
        printf("From Server: %s\n",buffer );
    // }
    return 0;
}