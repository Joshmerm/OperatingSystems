//12 Children
//Woken Up, Fed Breakfast, Sent To School, Dinner, Bath, Book, Book, Bed
//After Bath, Father DOes Book and Bed.
// Father Then Goes TO bed, And Then Wakes Up Wife.  (Cycle Repeats)
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

int day = 1;
int N = 0;
int count = 0;

sem_t fatherLock, motherLock;

void *father(void *buf)
{
    int last = 0;

    while (day <= N)
    {
        sem_wait(&fatherLock);
        for (int i = 1; i <= 12; i++)
        {
            cout << "Father is putting child #" << i << " To Bed" << endl;
        }
        cout << endl;
        cout << "Father is going to sleep and waking up mother to take care of the children." << endl;
        cout << endl;
        sem_post(&motherLock);
    }
    pthread_exit(0);
    // return 0;
}

void *mother(void *arg)
{

    while (day <= N)
    {
        sem_wait(&motherLock);

        cout << "This is day # " << day << " of a day in the life of Mother Hubbard" << endl;

        for (int i = 1; i < 13; i++)
            cout << "Child #" << i << " is being woken up." << endl;

        cout << endl;

        for (int i = 1; i < 13; i++)
            cout << "Child #" << i << " is being fed breakfast." << endl;
        cout << endl; 
        for (int i = 1; i < 13; i++)
            cout << "Child #" << i << " is being sent to school." << endl;
         
        cout << endl;
        
        for (int i = 1; i < 13; i++)
            cout << "Child #" << i << " is eating dinner." << endl;
        
        cout << endl;
        
        for (int i = 1; i < 13; i++)
            cout << "Child #" << i << " is getting a bath." << endl;
        
        cout << endl;
        
        sem_post(&fatherLock);
        day++;
    }
    pthread_exit(0);
    // return 0;
}

int main(int argc, char *argv[])
{
    N = atoi(argv[1]);

    pthread_t p, p2;

    sem_init(&motherLock, 1, 1);
    sem_init(&fatherLock, 1, 0);

    pthread_create(&p, NULL, &mother, NULL);
    pthread_create(&p2, NULL, &father, NULL);

    pthread_join(p, NULL);

    pthread_join(p2, NULL);

    sem_destroy(&motherLock);
    sem_destroy(&fatherLock);

    return 0;
}
