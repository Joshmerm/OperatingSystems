#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define Buffer_Limit 10
#define BUFFER_FULL false
#define BUFFER_EMPTY true
#define MAX_SIZE 5

pthread_mutex_t bufLock;

sem_t empty, full;

int count;


void consumer(char* buf){
    while(true){
        while(count == 0 || BUFFER_EMPTY)
            cout << "Waiting For Producer" << endl;
        // Do Nothing
        pthread_mutex_lock(&bufLock);
        cout << buf[count - 1] << endl;
        count--;
        pthread_mutex_unlock(&bufLock);
    }


}

void* producer(void* b){
    
    while (true){
        char* buf = (char*)b;
        while(count == MAX_SIZE || BUFFER_FULL)
            cout << "Waiting For Consumer" << endl;
        // Do Nothing
        pthread_mutex_lock(&bufLock);

        buf[count] = 'a' + rand()%26;
        count++;

        pthread_mutex_unlock(&bufLock);
    }
}

int main(){
   char buffer[MAX_SIZE];   
   pthread_t p;   
   count = 0;   
   pthread_mutex_init(&bufLock, NULL);   
   pthread_create(&p, NULL, &producer, &buffer); 
   pthread_join(p, NULL);  
   consumer(buffer);   
   return 0;
}