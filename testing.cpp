#include <iostream>
#include <fstream>

using namespace std;

int main() {
    ifstream input;
    ofstream output;

    input.open("source.txt");


    
    output.open("destination.txt");
    // getline (istream& is, string& str);
    cout << "Hello World!";
    return 0;
}