#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define ITER 1000000
#define MAX 30

void *thread_increment(void *arg);
void *thread_decrement(void *arg);

int x = 0;

sem_t m;
sem_t empty;
sem_t fill;

int main()
{
    pthread_t tid1, tid2;

    struct timespec start, end;

    sem_init(&m, 0, 1);
    sem_init(&empty, 0, MAX);
    sem_init(&fill, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_create(&tid1, NULL, thread_increment, NULL);
    pthread_create(&tid2, NULL, thread_decrement, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution Time: %.6f sec\n", elapsed);
    printf("Final x = %d\n", x);

    sem_destroy(&m);
    sem_destroy(&empty);
    sem_destroy(&fill);

    return 0;
}

void *thread_increment(void *arg)
{
    int i, val;

    for(i = 0; i < ITER; i++)
    {
        sem_wait(&empty);
        sem_wait(&m);

        val = x;
        x = val + 1;

        sem_post(&m);
        sem_post(&fill);
    }

    return NULL;
}

void *thread_decrement(void *arg)
{
    int i, val;

    for(i = 0; i < ITER; i++)
    {
        sem_wait(&fill);
        sem_wait(&m);

        val = x;
        x = val - 1;

        sem_post(&m);
        sem_post(&empty);
    }

    return NULL;
}
