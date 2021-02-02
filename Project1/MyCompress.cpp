#include <iostream>
#include <fstream>


using namespace std;

void compress (ifstream &in, ofstream &out){
    string line;
    string temp = "";
    string returnString;
    int count = 0;
    while (getline (in, line)) {
        for(int i = 0; i < line.size(); i++){
            if((line[i] - line[i + 1]) == 0){
                count++;
                temp += line[i];
            }else{
                if(count++ >= 15){
                    if(line[i] == '1') temp = "+" + to_string(count) + "+";
                    else temp = "-" + to_string(count) + "-";
                } 
                else{
                    temp += line[i];
                } 
                
                returnString += temp;
                count = 0;
                temp = "";
            }
        }
        returnString += '\n';
    }
    out << returnString;
    
}

int main(int argc, char** argv) { 
    ifstream in;
    ofstream out;
    // ofstream out;
    in.open(argv[1]);
    out.open(argv[2]);

    if(in.is_open() && out.is_open()){
        //Then do compression
        compress(in, out);
        in.close();
        out.close();
    }else{
        perror("Error");
        exit(1);
    }
  
    return 0; 
} 