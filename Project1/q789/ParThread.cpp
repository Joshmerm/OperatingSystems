#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <pthread.h>

std::ifstream input;
std::ofstream output;

//structure to keep track of arugument for thread function compress
struct arg_struct {
	int arg1;
	int arg2;
};

void* compress(void* argument) {

	struct arg_struct *args = (struct arg_struct *) argument;

	int startAt = args -> arg1;
	int end = args -> arg2;

	//for loop to read the line from the starting point to the end point
	for (int i = startAt; i <= end; i ++) {
		
		std::string text;
		std::string line;

        getline(input,text, ' ');

		int start = 0;
		int counter = 1;
		char symbol;
		
		for (int i = 0; i < text.length(); i ++) {

			if (i == 0) {
				if (text[i] == '0') {
					symbol = '-';
				}
				else {
					symbol = '+';
				}
			}
			else {

				if (text[i] == text[i-1]) {
					counter ++;
				}
				else {
					if (counter >= 16) {
						line += symbol + std::to_string(counter) + symbol;
					}
					else {
						for (int j = start; j < i; j ++) {
							line += text[j];
						}
					}
					start = i;
					counter = 1;
					if (text[i] == '0') {
						symbol = '-';
					}
					else {
						symbol = '+';
					}
				}
				
			}
			
		}

		//if read to the end of the line, count the rest of the remaining elements
		if (counter == (text.length() - start) && counter >= 16) {
			line += symbol + std::to_string(counter) + symbol;
		}
		else {
			for (int j = start; j < text.length(); j ++) {
				line += text[j];
			}
		}

		//write the line into output
		output << line;

		//if the input has element after the current, create a space or return based on the next line
		if (input.good()) {
			output << ' ';
		}
	}

	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    
    input.open(argv[1]);
    output.open(argv[2]);

	//reference reading file to keep track how many lines in the file
    std::ifstream inputCount(argv[1]);

    std::string temp;
    int count = 0;


	//create a vector to keep track of all the input separated by " " or return
	std::vector <std::string> countSize;
	

    while(getline(inputCount,temp)) {
		std::stringstream test(temp);
		std::string count;
        while (getline(test,count,' ')) {
			countSize.push_back("1"); // place holder to keep track the size
		}
    }


	count = countSize.size();

    inputCount.close();

    pthread_t tids[5];

	//divide the # of lines (with " " included) into 5 parts because of 5 threads
	int divise = count / 5;

	int process[] = {0, divise, (divise * 2), (divise * 3), (divise * 4), count};

	for (int i = 0; i < 5; i ++) {
		struct arg_struct args;
		args.arg1 = process[i];
		args.arg2 = process[i + 1];

        pthread_create(&tids[i], NULL, &compress, (void*) &args);
		
	}

	for (int i = 0; i < 5; i ++) {
		pthread_join(tids[i],NULL);
	}

	input.close();
	output.close();


	
	
}