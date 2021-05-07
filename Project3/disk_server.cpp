#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <sys/wait.h>
#include <sstream>
#include <time.h>

using namespace std;

#define portNum 9000

ifstream p_in;
ifstream d_in;

vector <string> xy;

vector <string> data;

string warning;

struct info{
    int socket;
    int cyr_num;
    int sector_num;
	int N;
    char* input;
};

bool isNumber(string s){
	for (int i = 0;i < s.length();i++){
		if (isdigit(s[i]) == false){
			return false;
		}
	}
	return true;
}

void* get_disk(void* arg){
    //Storing cylinder's number and sector's number into vector xy
    string temp;
    getline(p_in,temp);
    string tokenp;
    stringstream p(temp);
    while(getline(p,tokenp,' ')){
        xy.push_back(tokenp);
    }

    string temp2;
    while(getline(d_in,temp2)){
        data.push_back(temp2);
    }

    pthread_exit(0);
}

void * create_disk(void* arg){
    struct info* request = (struct info*)arg;
    int sock = request->socket;

    if (p_in.is_open()){
			warning = "Error, disk already exist";
				send(sock,warning.c_str(),warning.length(),0);
	}
	else{
				
		int cylinders = request->cyr_num;
		int sectors = request->sector_num;
		ofstream p_out("property.txt");
		ofstream d_out("disk.txt");
		string temp = "" + to_string(cylinders) + " " + to_string(sectors);
		p_out << temp;
		p_out.close();
		
        int size = cylinders * sectors;
			for (int i = 0;i < size;i++){
				d_out << endl;
			}			
			d_out.close();
			warning = "Disk created it successfully";
			send(sock,warning.c_str(),warning.length(),0);
		}
    
    pthread_exit(0);
}

void* delete_disk(void* arg){
    int sock = ((struct info*)arg) -> socket;

    if (p_in.is_open()){
		p_in.close();
		d_in.close();
		remove("property.txt");
		remove("disk.txt");
		warning = "Disk delete it successfully";
	}
	else{
		warning = "file doesn't exist";
	}
	send(sock,warning.c_str(),warning.length(),0);

    pthread_exit(0);
}

void* infomation_disk(void* arg){
    int sock = ((struct info*)arg) -> socket;
    
    string txtSize;
		if (p_in.is_open()){
				for (int i = 0;i < xy.size();i++){
                    txtSize += xy[i] + " ";
                }
				warning = txtSize;
		}
		else{
			warning = "file doesn't exist";
		}
		send(sock,warning.c_str(),warning.length(),0);

    pthread_exit(0);
}

void* read_disk(void* arg){
    struct info * request = (struct info*) arg;
    int sock = request->socket;
    int track = request->cyr_num;
    int sector = request->sector_num;
    
    int x = stoi(xy[0]);
    int y = stoi(xy[1]);
    int position = (track * y) - (y - sector);
    string temp;
	if (p_in.is_open() && track <= x && sector <= y){
		string ssize = data[position - 1];
		if (ssize.length() > 0){
			temp = "1 " + ssize;
		}
		else{
			temp = "0" + ssize;;
		}
	}
	else{
		temp = "Error, incorrect input";
	}
    
    send(sock,temp.c_str(),temp.length(),0);
    
    pthread_exit(0);
    
}

void* write_disk(void* arg){

	struct info * request = (struct info*) arg;
    int sock = request->socket;
    int track = request->cyr_num;
    int sector = request->sector_num;
	string sentence = request->input;
	
	int size = sentence.length();
	int x = stoi(xy[0]);
	int y = stoi(xy[1]);
	
	
	if (p_in.is_open() && track <= x && sector <= y && size <= 128){
		    int position = (track * y) - (y - sector);
			data[position - 1] = sentence;
			ofstream newdisk("disk.txt");
			for (int i = 0;i < data.size();i++){
				newdisk << data[i] << endl;
			}
			newdisk.close();
			warning = "1";
	}
	else{
		warning = "0";
	}
	
	send(sock,warning.c_str(),warning.length(),0);
		
	pthread_exit(0);
}
void* random_request(void * arg){
	srand(time(NULL));
	struct info * request = (struct info*) arg;
	int sock = request->socket;
	int ntimes = request->N;

	int x = stoi(xy[0]);
	int y = stoi(xy[1]);
	
	string seed = "Seed: ";
	for (int i = 0;i < ntimes;i++){
		int trackR = (rand() % x) + 1;
		int sectorS = (rand() % y) + 1;
		
		string sentence = "";
		int temp = (rand() % 2);
		int position = (trackR * y) - (y - sectorS);
		cout << temp << " " << position << endl;
		if (temp == 0){
			string size = data[position - 1];
		}
		else{
			for (int i = 0;i < 128;i++){
				char rch = 'a' + rand() % 26;
				sentence += rch;
			}
			data[position - 1] = sentence; 
			ofstream newdisk("disk.txt");
			for (int i = 0;i < data.size();i++){
				newdisk << data[i] << endl;
			}
			newdisk.close();
		}

		seed += to_string(temp) + " ";	
	}
	send(sock,seed.c_str(),seed.length(),0);

	pthread_exit(0);
}
int main(int argc,char ** argv)
{
    int server_fd, new_socket, valread;
	bool check = true;
	
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
	address.sin_port = htons(portNum);
	
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

    cout << "Server waiting... " << endl;

	while (check)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		cout << "Client connected" << endl;

        p_in.open("property.txt");
        d_in.open("disk.txt");

        pthread_t th;

        if (p_in.is_open() && d_in.is_open()){
            pthread_create(&th,NULL,&get_disk,NULL);
            pthread_join(th,NULL);
        }

		//buffer must create inside a while loop, otherwise it will store the previous command.
		//Very interesting, vector must be clear in the while loop after all the statement, otherwise, the vector "exit" command may not be working.
		char buffer[1024] = {0};
        
		string warning;
		read(new_socket,buffer,1024);
		vector <string> spliter;
    	string curr =buffer;
		stringstream p(curr);
		string t;
		while (getline(p,t,' ')){
			spliter.push_back(t);
		}

		
        struct info* arg = (struct info*)malloc(sizeof(struct info));

		if (spliter[0].compare("C") == 0){
			if (spliter.size() < 4){
				arg->socket = new_socket;
				arg->cyr_num = stoi(spliter[1]);
				arg->sector_num = stoi(spliter[2]);
				pthread_create(&th,NULL,&create_disk,(void *)arg);
				pthread_join(th,NULL);
			}
			else{
				warning = "Incorrect format, please try it again";
				send(new_socket,warning.c_str(),warning.length(),0);
			}	
		}
		else if (spliter[0].compare("D") == 0){
			if (spliter.size() < 2){
				arg->socket = new_socket;
				pthread_create(&th,NULL,&delete_disk,(void*)arg);
				pthread_join(th,NULL);
			}
			else{
				warning = "Incorrect format, please try it again";
				send(new_socket,warning.c_str(),warning.length(),0);
			}
		}
		else if (spliter[0].compare("I") == 0){
			if (spliter.size() < 2){
				arg->socket = new_socket;
				pthread_create(&th,NULL,&infomation_disk,(void*)arg);
				pthread_join(th,NULL);
			}
			else{
				warning = "Incorrect format, please try it again";
				send(new_socket,warning.c_str(),warning.length(),0);
			}		
		}
		else if (spliter[0].compare("R") == 0){
            if (spliter.size() < 4){
				arg->socket = new_socket;
            	arg->cyr_num = stoi(spliter[1]);
            	arg->sector_num = stoi(spliter[2]);
            	pthread_create(&th,NULL,&read_disk,(void*)arg);
            	pthread_join(th,NULL);
			}
			else{
				warning = "Incorrect format, please try it again";
				send(new_socket,warning.c_str(),warning.length(),0);
			}
			
		}
		else if (spliter[0].compare("W") == 0){
            arg->socket = new_socket;
            arg->cyr_num = stoi(spliter[1]);
            arg->sector_num = stoi(spliter[2]);
			//In case if there's a space between the words
            string completeInput = "";
            for (int i =3;i < spliter.size();i++){
                completeInput += spliter[i] + " ";				
            }

            //Convert completeinput into constnat string
            arg->input = (char *)completeInput.c_str();

            pthread_create(&th,NULL,&write_disk,(void*)arg);
            pthread_join(th,NULL);

		}
		else if (isNumber(spliter[0])){
			arg->socket = new_socket;
			arg->N = stoi(spliter[0]);
			pthread_create(&th,NULL,&random_request,(void*)arg);
            pthread_join(th,NULL);

		}
		else if (spliter[0].compare("exit") == 0){
			cout << "Connection terminated, thank you" << endl;
			check = false;
		}
		else{
			warning = "Unknown command, please try again\n";
			cout << "Undefine command, unable to access the command " << endl;
			send(new_socket,warning.c_str(),warning.length(),0);
		}
		warning.clear();
		spliter.clear();
		xy.clear();
	}
    return 0;
}