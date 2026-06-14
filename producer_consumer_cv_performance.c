#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define ITER 1000000
#define MAX 30

void *thread_increment(void *arg);
void *thread_decrement(void *arg);

int x = 0;

pthread_mutex_t m;
pthread_cond_t empty;
pthread_cond_t fill;

int main()
{
    pthread_t tid1, tid2;

    struct timespec start, end;

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill, NULL);

    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_create(&tid1, NULL, thread_increment, NULL);
    pthread_create(&tid2, NULL, thread_decrement, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution Time: %.6f sec\n", elapsed);
    printf("Final x = %d\n", x);

    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&fill);

    return 0;
}

void *thread_increment(void *arg)
{
    int i, val;

    for(i = 0; i < ITER; i++)
    {
        pthread_mutex_lock(&m);

        while(x == MAX)
        {
            pthread_cond_wait(&empty, &m);
        }

        val = x;
        x = val + 1;

        pthread_cond_signal(&fill);

        pthread_mutex_unlock(&m);
    }

    return NULL;
}

void *thread_decrement(void *arg)
{
    int i, val;

    for(i = 0; i < ITER; i++)
    {
        pthread_mutex_lock(&m);

        while(x == 0)
        {
            pthread_cond_wait(&fill, &m);
        }

        val = x;
        x = val - 1;

        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&m);
    }

    return NULL;
}
