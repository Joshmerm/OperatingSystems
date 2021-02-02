#include <iostream>
#include <fstream>
#include <string> 

using namespace std;

void decompress (ifstream &in, ofstream &out){
    string line;
    string temp;
    string returnString;
    string count;
    
    while (getline (in, line)) {
        for(int i = 0; i < line.size(); i++){
            if(line[i] == '+' || line[i] == '-'){
                bool isZero = line[i++] == '-';
                count = "";

                while(line[i] != '+' && line[i] != '-') count += line[i++];
                
                for(int i = 0; i < stoi(count); i++) temp += isZero ? '0' : '1';

                returnString += temp;
                temp = ""; 

            }
            else returnString += line[i];
        }
        returnString += '\n';
    }
    out << returnString;
    
}

int main(int argc, char** argv) { 
    ifstream in;
    ofstream out;
    in.open(argv[1]);
    out.open(argv[2]);

    if(in.is_open() && out.is_open()){
        //Then do compression
        decompress(in, out);
        in.close();
        out.close();
    }else{
        perror("Error");
        exit(1);
    }
  
    return 0; 
} 