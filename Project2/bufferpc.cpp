#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <iostream>

using namespace std;

#define THREAD_NUM 2
#define Max_SIZE 5

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

int buffer[Max_SIZE];
int count = 0;

void* producer(void* args) {
    while (1) {
         //Produce and generate random number
        sleep(1);
        
        sem_wait(&semEmpty);  //wait until semEmpty have at least one value and decrement
        pthread_mutex_lock(&mutexBuffer); //lock and protect the critical section
        //Added to the buffer
        buffer[count] = 'a' + rand() % 26;
        count++;
        pthread_mutex_unlock(&mutexBuffer); //unlock
        sem_post(&semFull);
    }
}

void* consumer(void* args) {

     //Always true, so it will keep communicate each other
    while (1) {
        char y;

        // Remove from the buffer
        sem_wait(&semFull); //wait until semFull have at least one value
        pthread_mutex_lock(&mutexBuffer); //lock and protect the critical section
        y = buffer[count - 1];
        count--;
        pthread_mutex_unlock(&mutexBuffer);  // release 
        sem_post(&semEmpty); // increment the value of the semaphore by 1

        // Consume
        sleep(1);
        cout << y << endl;
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    pthread_t th[THREAD_NUM];

    //Initialize the mutex and semaphore 
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (i > 0) {
            if (pthread_create(&th[i], NULL, &producer, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    //Destroy the terminal and mutex once we done
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}