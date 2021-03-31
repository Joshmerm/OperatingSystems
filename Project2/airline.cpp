#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <stdlib.h> 


using namespace std;

#define Buffer_Limit 10
#define BUFFER_FULL false
#define BUFFER_EMPTY true
#define MAX_SIZE 5

vector<int> waitingPassengers;
vector<int> waitingForHandlers;
vector<int> waitingForScreening;
vector<int> waitingToBeOnPlane;

pthread_mutex_t bufLock;

sem_t handlerLock, handlerLockRelease, screenerLock, screenerLockRelease, attendentLock, attendentLockRelease;

int count = 1;

int numberOfPassengers = 0;

int passengersProcessedHandlers = 0;
int passengersProcessedSecurity = 0;
int passengersProcessedAttendents = 0;

void* passenger(void* arg){
    int id = count++;
    string str = "Passenger #" + to_string(id) + " is waiting at baggage processing for a handler.\n";
    cout << str;
    sem_wait(&handlerLockRelease);
    //Now Being Processed
    sem_post(&handlerLock);

    string str2 = "Passenger #" + to_string(id) + " is waiting to be screened by a screener.\n";
    sem_wait(&screenerLockRelease);
    cout << str2;
    sem_post(&screenerLock);

    string str3 = "Passenger #" + to_string(id) + " is waiting to board the plane by an attendent.\n";
    sem_wait(&attendentLockRelease);
    cout << str3;
    sem_post(&attendentLock);

    string str4 = "Passenger #" + to_string(id) + " has been seated and relaxes.\n";
    cout << str4;


    pthread_exit(0);

}

void* handler(void* b){
    while(passengersProcessedHandlers++ < numberOfPassengers){
        sem_wait(&handlerLock);
        sem_post(&handlerLockRelease);
        // if(++passengersProcessedHandlers == numberOfPassengers) break;
    }
    pthread_exit(0);
}

void* screener(void* b){
    while(passengersProcessedSecurity++ < numberOfPassengers){
        sem_wait(&screenerLock);
        sem_post(&screenerLockRelease);
        // if(++passengersProcessedSecurity == numberOfPassengers) break;
    }
    pthread_exit(0);


}

void* attendent(void* b){
    while(passengersProcessedAttendents++ < numberOfPassengers){
        sem_wait(&attendentLock);
        sem_post(&attendentLockRelease);
        // if(++passengersProcessedAttendents == numberOfPassengers) break;
    }
    pthread_exit(0);


}





int main(int argc, char *argv[]){
    // int p = atoi(argv[1]), b = atoi(argv[2]), s = atoi(argv[3]), f = atoi(argv[4]);

    if(!atoi(argv[1]) || !atoi(argv[2]) || !atoi(argv[3]) || !atoi(argv[4])){
        perror("All Arguments Must Be Greater Than 0");
        exit(EXIT_FAILURE);
    }

    pthread_t baggage[atoi(argv[2])], screeners[atoi(argv[3])], attendants[atoi(argv[4])], passengers[atoi(argv[1])];

    numberOfPassengers = atoi(argv[1]);

    sem_init(&handlerLock, 1, atoi(argv[2]));
    sem_init(&handlerLockRelease, 1, atoi(argv[2]));
    sem_init(&screenerLock, 1, atoi(argv[3]));
    sem_init(&screenerLockRelease, 1, atoi(argv[3]));
    sem_init(&attendentLock, 1, atoi(argv[4]));
    sem_init(&attendentLockRelease, 1, atoi(argv[4]));

    for(int i = 1; i <= atoi(argv[1]); i++)
        waitingPassengers.push_back(i);

    for(int i = 0; i < atoi(argv[2]); i++)
        pthread_create(&baggage[i], NULL, &handler, NULL);

    for(int i = 0; i < atoi(argv[3]); i++)
        pthread_create(&screeners[i], NULL, &screener, NULL);
    
    for(int i = 0; i < atoi(argv[4]); i++)
        pthread_create(&attendants[i], NULL, &attendent, NULL);

    for(int i = 0; i < atoi(argv[1]); i++)
        pthread_create(&passengers[i], NULL, &passenger, NULL);

    // SPACE


    for(int i = 0; i < atoi(argv[2]); i++)
        pthread_join(baggage[i], NULL);

    for(int i = 0; i < atoi(argv[3]); i++)
        pthread_join(screeners[i], NULL);

    for(int i = 0; i < atoi(argv[4]); i++)
        pthread_join(attendants[i], NULL);


    for(int i = 0; i < atoi(argv[1]); i++)
        pthread_join(passengers[i], NULL);

    // pthread_join(p2, NULL);


    sem_destroy(&handlerLock);
    sem_destroy(&handlerLockRelease);
    sem_destroy(&screenerLock);
    sem_destroy(&screenerLockRelease);
    sem_destroy(&attendentLock);
    sem_destroy(&attendentLockRelease);


   return 0;
}