#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#define PORT 8080


int main(int argc, char *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    // menu list
    std::cout << "Welcome to disk client, type in the following command to manage a virtual disk space!\n";
    std::cout << "[F]ormat a new file system\n";
    std::cout << "[RESET] to delete a current file system\n";
    std::cout << "[L]ist the current file directory with option [0/1] (ex. L 0 or L 1)\n";
    std::cout << "[R]ead a specific file under the current directory (ex. R test.txt)\n";
    std::cout << "[W]rite a peice of data to a specific file (ex. W test.txt hello)\n";
    std::cout << "[C]reate a file with .txt extension only under the current directory (ex. C test.txt (it must contain an extension))\n";
    std::cout << "[D]elete a file untder the current directory (ex. D test.txt)\n";
    std::cout << "[mkdir] to make a directory (ex. mkdir test)\n";
    std::cout << "[cd] to change directory (ex. cd test or .. to go back up one directory or ... back to the root file)\n";
    std::cout << "[pwd] to show the current working path (ex. pwd)\n";
    std::cout << "[rmdir] to remove a directory (ex. rmdir test)\n";
    std::cout << "[exit] to exit the client\n";
    std::cout << "When you see 0 before the data, it means processed successfully; if 1, it means processed failed; if 2, fail due to special issues\n";
    std::cout << "------------------------------------------------------------\n";

    // as long as the user do not want to quit the client, the client will generate a new socket for every request
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
        char buffer[32768] = {0};
        std::cout << ">";
        getline(std::cin, input);
        send(sock, input.c_str(), input.length(), 0);
        if (input.compare("exit") == 0) {
             return 0;
        }

        valread = read(sock, buffer, 32768);
        std::cout << buffer << "\n";
    }



    
    
    return 0;



}