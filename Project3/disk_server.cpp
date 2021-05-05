#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <sstream>

#define PORT 8080

std::ifstream in_disk;
int track_sector[2];

std::vector<std::string> datas;

std::string warnning;

// read formula
// ( (track - 1) * sector ) + (sector_read)

struct info{
    int socket;
    int track_num;
    int sector_num;
    char* input;
};

void* get_disk(void* request) {
        std::string temp;
        while(getline(in_disk, temp)) {
            datas.push_back(temp);
        }
        std::stringstream s(datas[0]);
        short count = 0;
        while (getline(s, temp, ' ')) {
            track_sector[count] = std::stoi(temp);
            count ++;
        }

        pthread_exit(0);
}

void* information_request(void* request) {
    int sock = ((struct info*) request)->socket;

    if (in_disk.is_open()) {
        warnning = std::to_string(track_sector[0]) + " " + std::to_string(track_sector[1]);
        send(sock, warnning.c_str(), warnning.length(), 0);
    }
    else {
        warnning = "Error, disk not exist, cannot retrieve disk information.";
        std::cout << "Disk does not exist\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
    }
    
    pthread_exit(0);
}

void* create_disk(void* request) {
    struct info* input = (struct info*) request;
    int sock = input->socket;


    if (in_disk.is_open()) {
        warnning = "Error, disk already existed";
        std::cout << "There is already a disk\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
    }
    else {
        int tracks = input->track_num;
        int sectors = input->sector_num;
        std::ofstream newDisk("disk.txt");
        newDisk << std::to_string(tracks) + " " + std::to_string(sectors) + "\n";
        for (int i = 0; i < (tracks * sectors); i ++) {
            newDisk << "\n";
        }
        warnning = "Success, disk created";
        std::cout << "Disk Created\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
        newDisk.close();
    }

    
    pthread_exit(0);
}

void* delete_disk(void* request) {
    int sock = ((struct info*) request)->socket;

    if (in_disk.is_open()) {
        in_disk.close();
        remove("disk.txt");
        warnning = "Remove sucessfully";
        std::cout << "Disk removed\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
    }
    else {
        warnning = "No disk to remove";
        std::cout << "Fail to remove because there is no disk\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
    }

    pthread_exit(0);
}

void* read_disk(void* request) {
    struct info* input = (struct info*) request;
    int sock = input->socket;
    int tracks = input->track_num;
    int sectors = input->sector_num;

    if (in_disk.is_open() && tracks <= track_sector[0] && sectors <= track_sector[1] && 
        !(tracks <= 0) && !(sectors <= 0)) {
        int position = ((tracks - 1) * track_sector[1]) + (sectors);
        warnning = "1" + datas[position];
        std::cout << "Return value to client\n";
        send(sock, warnning.c_str(), warnning.length(), 0);

    }
    else {
        warnning = "0";
        std::cout << "Fail to provide information back to the client (probably no disk or invalid input)\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
    }

    
    pthread_exit(0);
}


void* write_disk(void* request) {
    struct info* input = (struct info*) request;
    int sock = input->socket;
    int tracks = input->track_num;
    int sectors = input->sector_num;
    std::string words = input->input;

    int input_length = words.length();

    if (in_disk.is_open() && tracks <= track_sector[0] && sectors <= track_sector[1] && 
        input_length <= 128 && !(tracks <= 0) && !(sectors <= 0)) {
        int position = ((tracks - 1) * track_sector[1]) + (sectors);
        datas[position] = words;
        std::ofstream newDisk("disk.txt");
        newDisk << std::to_string(track_sector[0]) + " " + std::to_string(track_sector[1]) + "\n";
        for (int i = 0; i < (track_sector[0] * track_sector[1]); i ++) {
            newDisk << datas[i+1] << "\n";
        }
        newDisk.close();
        warnning = "1";
        std::cout << "Write sucesscefully\n";
        send(sock, warnning.c_str(), warnning.length(), 0);

    }
    else {
        warnning = "0";
        std::cout << "Fail to write the data (probably no disk or invalid input)\n";
        send(sock, warnning.c_str(), warnning.length(), 0);
    }    

    pthread_exit(0);
}

int main(int argc, char *argv[]) {

    int server_fd, new_socket, valread;

    struct sockaddr_in address;

    int opt = 1;
    int addrlen = sizeof(address);

    

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Server connected to port " << PORT << "\n";
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }



    while(true) {

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *) &addrlen))< 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "Client Connected\n";
        // open the disk
        in_disk.open("disk.txt");

        pthread_t pt;
        // if the disk is existed, read the number of tracks and sectors and datas
        if (in_disk.is_open()) {
            pthread_create(&pt, NULL, &get_disk, NULL);

            pthread_join(pt, NULL);
        }

        char buffer[1024] = {0};

        read(new_socket, buffer, 1024);

        std::cout << "Received \"" << buffer << "\" from client\n";

        std::vector<std::string> command_list;
        std::string conv = buffer;
        std::stringstream ex(conv);
        std::string temp;

        while (getline(ex, temp, ' ')) {
            command_list.push_back(temp);
        }

        struct info* arg = (struct info*)malloc(sizeof(struct info));

        if (command_list[0].compare("I") == 0) {
            arg->socket = new_socket;
            pthread_create(&pt,NULL, &information_request, (void *) arg);
            pthread_join(pt, NULL);

        }
        else if (command_list[0].compare("C") == 0) {
            arg->socket = new_socket;
            arg->track_num = std::stoi(command_list[1]);
            arg->sector_num = std::stoi(command_list[2]);
            pthread_create(&pt,NULL, &create_disk, (void *) arg);

            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("D") == 0) {
            arg->socket = new_socket;
            pthread_create(&pt, NULL, &delete_disk, (void *) arg);

            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("R") == 0) {
            arg->socket = new_socket;
            arg->track_num = std::stoi(command_list[1]);
            arg->sector_num = std::stoi(command_list[2]);
            pthread_create(&pt,NULL, &read_disk, (void *) arg);

            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("W") == 0) {
            arg->socket = new_socket;
            arg->track_num = std::stoi(command_list[1]);
            arg->sector_num = std::stoi(command_list[2]);
            std::string completeInput = "";
            for (int i = 3; i < command_list.size(); i ++) {
                completeInput += command_list[i];
                if (i != (command_list.size() - 1)) {
                    completeInput += " ";
                }
            }

            arg->input = (char *) completeInput.c_str();
            pthread_create(&pt,NULL, &write_disk, (void *) arg);

            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("exit") == 0) {
            std::cout<< "The client has exited\n";
        }
        else {
            warnning = "Unknown command, please try again (input case is sensitive)";
            std::cout << "Unknown command received from the client, no information return\n";
            send(new_socket, warnning.c_str(), warnning.length(), 0);
        }

        if (in_disk.is_open()) {
            in_disk.close();
        }

        datas.clear();

    }


    return 0;


}