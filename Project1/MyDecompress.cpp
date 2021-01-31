#include <iostream>
#include <fstream>
#include <sstream> 

using namespace std;

void compress (ifstream &in, ofstream &out){
    string line;
    string temp;
    string returnString;
    
    while (getline (in, line)) {
        for(int i = 0; i < line.size(); i++){
            // cout << "in";
            if(line[i] == '+' || line[i] == '-'){
                bool isZero = line[i] == '-';
                i++;
                while(line[i] != '+' && line[i] != '-'){
                   temp += line[i++];
                }

                stringstream geek(temp);
                temp = ""; 
                int n = 0;
                geek >> n;
                // cout << geek <s<  endl;
                for(int i = 0; i < n; i++){
                    temp += isZero ? '0' : '1';
                }
                returnString += temp;
                temp = ""; 

            }else{
                returnString += line[i];
            }
        }
        returnString += "\n";
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
        compress(in, out);
        in.close();
        out.close();
    }else{
        cout << "Error, Couldnt open\n";
        exit(1);
    }
  
    return 0; 
} 