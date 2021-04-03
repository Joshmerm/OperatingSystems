#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <vector>

#define WAIT_TIME 1000000
pthread_mutex_t arriveCheck,bagCheck, secureCheck,faCheck, print;
sem_t bag, secure, fa, waiting;

// tracking the work of the thread
int arrive = 0;
int pass = 0;
int securepass = 0;
int ready2go = 0;
//over all amoung of passenger
int overall;
std::vector<int> list; //representing selection list, the order of receiving service

void* fas(void* arg) {

    while (ready2go < overall) {        
        sem_wait(&fa); // wait until a passenger pass the previous section
        
        pthread_mutex_lock(&faCheck); //protect the criticacl section
        
        if(ready2go >= overall) { //if inside of the critical section to prevent calculation error
            pthread_mutex_unlock(&faCheck);
            break;
        }

        
        // usleep(WAIT_TIME);

        int current = list[ready2go];

        pthread_mutex_lock(&print);
        std::cout << "Passenger #" << current << " is ready to go\n";
        pthread_mutex_unlock(&print);
        ready2go += 1;
        pthread_mutex_unlock(&faCheck);

    }

    //an extra post to release other threads that is running at the same time
    sem_post(&fa);

    pthread_exit(NULL);
}

void* securityScan(void* arg) {
    while (securepass < overall) {
        sem_wait(&secure);
       
        pthread_mutex_lock(&secureCheck);
        
        if(securepass >= overall) {
            pthread_mutex_unlock(&secureCheck);
            break;
        }
        
        // usleep(WAIT_TIME);

        int current = list[securepass];

        pthread_mutex_lock(&print);
        std::cout << "Passenger #" << current << " pass the scurity scan and waiting to get on the plane.\n";
        pthread_mutex_unlock(&print);
        
        securepass += 1;
        pthread_mutex_unlock(&secureCheck);
        sem_post(&fa);
    }

     //an extra post to release other threads that is running at the same time
    sem_post(&secure);

    pthread_exit(NULL);
}

void* baggage(void* arg) {
    while (arrive < overall) {
        sem_wait(&bag);
        
        pthread_mutex_lock(&bagCheck);
        
        if(arrive >= overall) {
            pthread_mutex_unlock(&bagCheck);
            break;
        }
        
        

        int current = list[arrive];
        
        pthread_mutex_lock(&print);
        std::cout << "Passenger #" << current << " pass the baggage check and preparing for security scan.\n";
        pthread_mutex_unlock(&print);
        
        arrive += 1;
        pthread_mutex_unlock(&bagCheck);
        sem_post(&secure);
    }

     //an extra post to release other threads that is running at the same time
    sem_post(&bag);

    pthread_exit(NULL);
}

void* passgate(void* arg) {
    int j; //local variable j to represent passenger id
 
    pthread_mutex_lock(&arriveCheck);
    pass += 1; //global variable
    j = pass; // as soon as pass increase, j retrieve pass
    pthread_mutex_unlock(&arriveCheck);
    
    pthread_mutex_lock(&print);
    std::cout << "Passenger #" << j << " arrive and waiting for the baggae check.\n";    
    pthread_mutex_unlock(&print);

    //wait until all the passenger arrive before start working
    sem_wait(&waiting);
    //all the passenger will fight for the first to serve
    list.push_back(j);
    sem_post(&waiting);

    //tell the baggager there is a passenger coming
    sem_post(&bag);

    pthread_exit(NULL);
}



int main (int argc, char* argv[]) {

    // transform string into integer based on the user input command line argument
    int nPass;
    int nBag;
    int nSecure;
    int nFa;

    try {
        nPass = std::stoi(argv[1]);
        nBag = std::stoi(argv[2]);
        nSecure = std::stoi(argv[3]);
        nFa = std::stoi(argv[4]);

        if (nPass <= 0 || nBag <= 0 || nSecure <= 0 || nFa <= 0) {
            throw std::invalid_argument("");
        }
    }
    catch (std::invalid_argument) {
        std::cout << "Invalid input, program exit\n";
        return 0;
    }
    catch (std::logic_error) {
        std::cout << "Not enough input for command line argument, program exit\n";
        return 0;
    }


    //overall passenger the program needs to take care
    overall = nPass;


    //"staff" avalibility
    sem_init(&bag, 0, 0);
    sem_init(&secure, 0, 0);
    sem_init(&fa, 0, 0);
    sem_init(&waiting, 0, 0);

    //mutex lock on each section to protect the critical resource
    pthread_mutex_init(&arriveCheck, NULL);
    pthread_mutex_init(&bagCheck, NULL);
    pthread_mutex_init(&secureCheck, NULL);
    pthread_mutex_init(&faCheck, NULL);
    pthread_mutex_init(&print, NULL);

    //array of threads, indicate the number of "staff" for each position
    pthread_t baggager[nBag];
    pthread_t passenger[nPass];
    pthread_t security[nSecure];
    pthread_t flightattendent[nFa];

    //create and join back the threads
    for (int i = 0; i < nBag; i ++) {
        pthread_create(&baggager[i], NULL, &baggage, NULL);
    }
    for (int i = 0; i < nSecure; i ++) {
        pthread_create(&security[i], NULL, &securityScan, NULL);
    }
    for (int i = 0; i < nFa; i ++) {
        pthread_create(&flightattendent[i], NULL, &fas, NULL);
    }



    for (int i = 1; i <= nPass; i ++) {
        pthread_create(&passenger[i-1], NULL, &passgate, NULL);
    }
    sem_post(&waiting);

    for (int i = 0; i < nBag; i ++) {
        pthread_join(baggager[i],NULL);
        
    }

    for (int i = 0; i < nSecure; i ++) {
        pthread_join(security[i],NULL);
    }

    for (int i = 0; i < nFa; i ++) {
        pthread_join(flightattendent[i],NULL);
    }

    for (int i = 0; i < nPass; i ++) {
        pthread_join(passenger[i],NULL);
    }
}