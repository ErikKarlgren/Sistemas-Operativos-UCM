#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];
pthread_mutex_t forks[NR_PHILOSOPHERS];

void init()
{
    int i;
    for (i = 0; i < NR_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i], NULL);
}

void think(int i)
{
    printf("Philosopher %d thinking... \n", i);
    sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n", i);
}

void eat(int i)
{
    printf("Philosopher %d eating... \n", i);
    sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n", i);
}

void toSleep(int i)
{
    printf("Philosopher %d sleeping... \n", i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n", i);
}

// one philosopher should grab first the fork on the left, and then the one on the right
// the other ones should follow the opposite order
void *philosopher(void *i)
{
    unsigned long nPhilosopher = (unsigned long)i;
    unsigned long right = nPhilosopher;
    unsigned long left = (nPhilosopher - 1 == -1UL) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    while (1)
    {
        think(nPhilosopher);

        /// TRY TO GRAB BOTH FORKS (right and left)
        pthread_mutex_lock(nPhilosopher == 0 ? &forks[left] : &forks[right]);
        pthread_mutex_lock(nPhilosopher == 0 ? &forks[right] : &forks[left]);

        eat(nPhilosopher);  

        // PUT FORKS BACK ON THE TABLE
        // we put them in reverse order
        pthread_mutex_unlock(nPhilosopher == 0 ? &forks[right] : &forks[left]);
        pthread_mutex_unlock(nPhilosopher == 0 ? &forks[left] : &forks[right]);

        toSleep(nPhilosopher);
    }
}

int main()
{
    init();
    unsigned long i;
    for (i = 0; i < NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void *)i);

    for (i = 0; i < NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);
    return 0;
}
