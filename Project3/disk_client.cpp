#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#define PORT 8080


int main(int argc, char *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    std::cout << "Welcome to disk client, type in the following command to manage a virtual disk space!\n";
    std::cout << "[C]reate a disk with X tracks and Y sectors (ex. C 10 8)\n";
    std::cout << "[D]elete a current exist disk\n";
    std::cout << "[I]nform the properties (X trakcs and Y sectors) of the current disk\n";
    std::cout << "[R]ead a specific data from track X sector Y (ex. R 2 6)\n";
    std::cout << "[W]rite a peice of data to a specic sector of a track (ex. W 1 2 hello)\n";
    std::cout << "[exit] to exit the client\n";
    std::cout << "When you see 1 before the data, it means processed successfully; if 0, it means processed failed\n";
    std::cout << "------------------------------------------------------------\n";

    while(true) {

        if ((sock = socket(AF_INET, SOCK_STREAM, 0))< 0) {
            std::cout << "Socket creation fail\n";
            return -1;
        }
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);


        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            std::cout << "Invalid address/ Address is not supported\n";
            return -1;
        }
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cout << "connection failed\n";
            return -1;
        }

        std::string input;
        char buffer[1024] = {0};
        std::cout << ">";
        getline(std::cin, input);
        send(sock, input.c_str(), input.length(), 0);
        if (input.compare("exit") == 0) {
            break;
        }

        valread = read(sock, buffer, 1024);
        std::cout << buffer << "\n";
    }



    
    
    return 0;



}