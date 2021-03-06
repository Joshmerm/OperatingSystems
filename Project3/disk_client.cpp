#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

using namespace std;

#define portNum 9000

bool isNumber(string s){
	for (int i = 0;i < s.length();i++){
		if (isdigit(s[i]) == false){
			return false;
		}
	}
	return true;
}
int main(int argc, char ** argv)
{
    int client = 0, valread;

    struct sockaddr_in serv_addr;
	cout << "C : Enter C c s will create a virtual disk file(Size of c * s)" << endl;
	cout << "D : Delte the virtual disk" << endl;
	cout << "I: Enter I, display the overall size of the cylinder and vector" << endl;
    cout << "R: Enter R c s will show you the content of this specific position" << endl;
    cout << "W: Enter W c s l will writing data into that specific position " << endl;
	cout << "exit : Quit the system" << endl;
	cout << "(c : cylinder's position, s: vector's position)" << endl;
	cout << "(l:the number of bytes it provide, it's size shouldn't large than 128 bytes)" << endl;
    cout << endl;

	while (true){

	if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portNum);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

		/*
			General idea: Getline to catch user's input and used if-else statement to check
			if the user input is correct or not. 
			isNumber : A method use to check user's input is digit/number or not
			Most likely, it accept command of create, delete, information of disk,exit
			read and write specfic location of the disk.

		*/

    	string input;
		char buffer[1024] = {0};
		cout << "Command: ";
    	getline(cin,input);

		if (!(isNumber(input))){
			strcpy(buffer,input.c_str());
			send(client ,buffer , strlen(buffer) , 0 );
			if (input.compare("exit") == 0){
				break;
			}
			valread = read( client , buffer, 1024);

			
		}
		else{
			string wrong = "error";
			send(client ,wrong.c_str(),wrong.length() , 0 );
			valread = read( client , buffer, 1024);
		}
		
		cout << buffer << endl;
		
	}
    
	return 0;
}

