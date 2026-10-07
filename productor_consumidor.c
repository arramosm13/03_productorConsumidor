#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFF_SIZE 5
#define NP 3
#define NC 3
#define NITERS 4

typedef struct {
    int buf[BUFF_SIZE];
    int in;
    int out;
    sem_t full;
    sem_t empty;
    sem_t mutex;
} sbuf_t;

sbuf_t shared;

void *Producer(void *arg)
{
    int i, elemento, index;

    index = (int)(long)arg;

    for (i = 0; i < NITERS; i++) {
        elemento = i;

        sem_wait(&shared.empty);
        sem_wait(&shared.mutex);

        shared.buf[shared.in] = elemento;
        shared.in = (shared.in + 1) % BUFF_SIZE;
        printf("[P%d] Produciendo %d ...\n", index, elemento);
        fflush(stdout);

        sem_post(&shared.mutex);
        sem_post(&shared.full);

        if (i % 2 == 1) sleep(1);
    }
    return NULL;
}

void *Consumer(void *arg)
{
    int i, elemento, index;

    index = (int)(long)arg;

    for (i = 0; i < NITERS; i++) {
        sem_wait(&shared.full);
        sem_wait(&shared.mutex);

        elemento = shared.buf[shared.out];
        shared.out = (shared.out + 1) % BUFF_SIZE;
        printf("------> [C%d] consumió %d\n", index, elemento);
        fflush(stdout);

        sem_post(&shared.mutex);
        sem_post(&shared.empty);

        if (i % 2 == 1) sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t idP[NP], idC[NC];
    long index;

    shared.in = 0;
    shared.out = 0;

    sem_init(&shared.full, 0, 0);
    sem_init(&shared.empty, 0, BUFF_SIZE);
    sem_init(&shared.mutex, 0, 1);

    for (index = 0; index < NP; index++)
    {
        pthread_create(&idP[index], NULL, Producer, (void*)index);
    }

    for (index = 0; index < NC; index++)
    {
        pthread_create(&idC[index], NULL, Consumer, (void*)index);
    }

    for (index = 0; index < NP; index++)
    {
        pthread_join(idP[index], NULL);
    }
    for (index = 0; index < NC; index++)
    {
        pthread_join(idC[index], NULL);
    }

    sem_destroy(&shared.full);
    sem_destroy(&shared.empty);
    sem_destroy(&shared.mutex);

    return 0;
}




