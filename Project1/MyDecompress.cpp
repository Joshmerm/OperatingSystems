#include <iostream>
#include <fstream>
#include <string> 

using namespace std;

void decompress (ifstream &in, ofstream &out){
    //line keeps track of the specific line the process is at
    //Temp holds 0 or 1 when decompressing when it finds either a '-' or '+' respectively
    //return string the the decompressed string version of the file, count is the decomrpessed version number of 0's or 1's
    string line;
    string temp;
    string returnString;
    string count;
    
    //iterate through each line in the file
    while (getline (in, line)) {
        //iterate through each character in the line
        for(int i = 0; i < line.size(); i++){
            //if the character is a '+' or '-' then we decompress
            if(line[i] == '+' || line[i] == '-'){
                //check to see if it is a zero we are looking at or a one & reset count, and then go to the next character
                bool isZero = line[i++] == '-';
                count = "";
                //saved the string number thats in between the '+' or '-'
                while(line[i] != '+' && line[i] != '-') count += line[i++];
                //convert the string version count, to a number and iterate that many times, adding a 0 or 1 resepectively
                for(int j = 0; j < stoi(count); j++) temp += isZero ? '0' : '1';

                //add temp to returnstring, reset temp
                returnString += temp;
                temp = ""; 

            }
            //it is just a normal 1 or 0, doesnt need any special operations
            else returnString += line[i];
        }
        //add an endline after going through the entire line
        returnString += '\n';
    }
    //output returnstring to the output path
    out << returnString;
    
}

//Takes in Two command Line Arguments (input file path, output file path)
//Example for running: ./MyCompress source.txt destination.txt 
int main(int argc, char** argv) { 
    //open the input file, and output file
    ifstream in;
    ofstream out;
    in.open(argv[1]);
    out.open(argv[2]);

    //if successful, decompress
    if(in.is_open() && out.is_open()){
        //Then do compression
        decompress(in, out);
        in.close();
        out.close();
    }else{
        //error opening files
        perror("Error");
        exit(1);
    }
  
    return 0; 
} 