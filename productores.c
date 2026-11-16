#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCTORES 2
#define NUM_CONSUMIDORES 2
#define ITEMS_POR_HILO 6

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t vacios;
sem_t llenos;
pthread_mutex_t mutex;

void* productor(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_POR_HILO; i++) {
        int item = (id * 100) + i + 1;
        sleep(rand() % 2 + 1);

        sem_wait(&vacios);
        pthread_mutex_lock(&mutex);

        buffer[in] = item;
        printf("[Productor %d] Produjo: %d en posición %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&llenos);
    }

    printf(">>> Productor %d ha terminado su producción. <<<\n", id);
    pthread_exit(NULL);
}

void* consumidor(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_POR_HILO; i++) {
        sem_wait(&llenos);
        pthread_mutex_lock(&mutex);

        int item = buffer[out];
        printf("\t[Consumidor %d] Consumió: %d de posición %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&vacios);

        sleep(rand() % 2 + 1);
    }

    printf(">>> Consumidor %d ha terminado de consumir. <<<\n", id);
    pthread_exit(NULL);
}

int main() {
    pthread_t prod[NUM_PRODUCTORES], cons[NUM_CONSUMIDORES];
    int id_prod[NUM_PRODUCTORES], id_cons[NUM_CONSUMIDORES];

    srand(time(NULL));

    sem_init(&vacios, 0, BUFFER_SIZE);
    sem_init(&llenos, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        id_prod[i] = i + 1;
        if (pthread_create(&prod[i], NULL, productor, &id_prod[i]) != 0) {
            perror("Error al crear productor");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        id_cons[i] = i + 1;
        if (pthread_create(&cons[i], NULL, consumidor, &id_cons[i]) != 0) {
            perror("Error al crear consumidor");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(prod[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(cons[i], NULL);
    }

    sem_destroy(&vacios);
    sem_destroy(&llenos);
    pthread_mutex_destroy(&mutex);

    printf("\nEjecución finalizada correctamente.\n");
    return 0;
}
