#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

using namespace std;

#define portNum 9000

//A method used to check a string is number or not
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

	cout << "I: display the overall size of the cylinder and vector" << endl;
    cout << "N: Enter any number and it will generate equal amount of read/write request" << endl;
    cout << "exit : Quit the system" << endl;
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
			Command Accept : I for information, N for any number, to generate equal amount of random read/write request
			exit, for shut down the system.
		*/
    	string input;
		char buffer[1024] = {0};
		cout << "Command: ";
    	getline(cin,input);

		if ((isNumber(input)) || ((input.compare("I") == 0) || (input.compare("exit") == 0))){
			strcpy(buffer,input.c_str());
			send(client ,buffer , strlen(buffer) , 0 );
			if (input.compare("exit") == 0){
				break;
			}

		}
		else{
			string wrong = "error";
			send(client ,wrong.c_str(),wrong.length() , 0 );
			
		}
		valread = read( client , buffer, 1024);
		cout << buffer << endl;
		
	}
    
	return 0;
}