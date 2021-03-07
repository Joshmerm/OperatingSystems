#include <iostream>
#include <fstream>


using namespace std;

void compress (ifstream &in, ofstream &out);

//Takes in Two command Line Arguments (input file path, output file path)
//Example for running: ./MyCompress source.txt destination.txt 
int main(int argc, char** argv) { 
    //Open both input and output files using command line arguments
    ifstream in;
    ofstream out;
    in.open(argv[1]);
    out.open(argv[2]);

    if(in.is_open() && out.is_open()){
        //Then do compression
        compress(in, out);
        //close both files
        in.close();
        out.close();
    }else{
        //error opening files
        perror("Error Opening Files");
        exit(1);
    }
  
    return 0; 
} 

void compress (ifstream &in, ofstream &out){
    //line is each line of the file, temp is used to save each character of the line in the case that
    //count isnt larger than 15 (will explain below why 15)
    //returnstring is used to save the compressed file and output to the output path
    string line;
    string temp = "";
    string returnString;
    int count = 0;

    //iterates through each line of the input file
    while (getline (in, line)) {
        //runs through each character in the line
        for(int i = 0; i < line.size(); i++){
            //checks to see if the the current character and character next to it are the same
            //if yes, then count is incremented, and we save the current character to the temp string
            if((line[i] - line[i + 1]) == 0){
                count++;
                temp += line[i];
            //if they are different, we check to see if count is greater than 15, this is because we havent incremeneted
            //the [line i + 1] previously, we only checked current matched with the next,
            //this line also increments so we still count the current
            }else{
                if(count++ >= 15){
                    //saves a '+' for 1 and a '-' for 0, convert count to a string and add to the temp string 
                    if(line[i] == '1') temp = "+" + to_string(count) + "+";
                    else temp = "-" + to_string(count) + "-";
                } 
                else{
                    //if count is not >= 15, we  add the character to temp
                    temp += line[i];
                } 
                
                //whatever is stored in temp, is then added to return string, and we reset count & temp
                returnString += temp;
                count = 0;
                temp = "";
            }
        }
        //after going through each character in a line, we know that there is a endline, so we add it back
        returnString += '\n';
    }
    //output return string to the output path
    out << returnString;
    
}