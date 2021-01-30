#include <iostream>
#include <fstream>
#include<string>

using namespace std;

void compress (ifstream &in, ofstream &out){
    string line;
    string temp;
    string returnString;
    bool isZero = false;
    int count = 0;
    while (getline (in, line)) {
        // Output the text from the file
        // cout << line << "\n";
        // cout << line.size() << "\n";
        //go through each character of the string
        for(int i = 0; i < line.size() - 1; i++){
            // cout << "in";
            if(line[i] == '1' && line[i + 1] == '1'){
                count++;
                isZero = false;
                temp += line[i];
            }else if(line[i] == '0' && line[i + 1] == '0'){
                count++;
                isZero = true;
                temp += line[i];
                // cout << "in";
            }else{
                if(count++ >= 15){
                    // cout << "in";
                    if(!isZero){
                        temp = "+" + to_string(count) + "+";
                    }else{
                        temp = "-" + to_string(count) + "-";
                    }

                }else{
                    temp += line[i];
                }
                count = 0;
                returnString += temp;
                temp = "";
            }
        }
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