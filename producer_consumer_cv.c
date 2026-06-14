#include <stdio.h>
#include <pthread.h>

#define ITER 1000
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

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill, NULL);

    pthread_create(&tid1, NULL, thread_increment, NULL);
    pthread_create(&tid2, NULL, thread_decrement, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("\nFinal x = %d\n", x);

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

        printf("[Producer] %lu: %d\n", (unsigned long)pthread_self(), val);

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

        printf("[Consumer] %lu: %d\n", (unsigned long)pthread_self(), val);

        x = val - 1;

        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&m);
    }

    return NULL;
}
