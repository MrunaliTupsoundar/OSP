// file_producer_consumer.c — Final Solution for 4.3 (mutex + binary semaphores)
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>   // for usleep
#include <time.h>     // for rand, srand

// --- Configuration ---
#define TOTAL_ITEMS 50
#define FILENAME "nums.txt"

// --- Synchronization Tools ---
pthread_mutex_t file_mutex;      // Protects the shared file
sem_t value_ready;               // Producer → Consumer signal
sem_t value_retrieved;           // Consumer → Producer signal

// --- Producer Thread ---
void* producer(void* arg) {
    (void)arg; // unused
    FILE* fp = fopen(FILENAME, "w"); // Create or truncate file
    if (!fp) {
        perror("Producer fopen");
        return NULL;
    }

    printf("Producer started — generating %d random numbers (every 0.5s)...\n", TOTAL_ITEMS);

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        usleep(500000); // fixed delay 0.5 seconds

        int num = rand() % 10; // random number 0–9

        pthread_mutex_lock(&file_mutex);
        fprintf(fp, "%d\n", num);
        fflush(fp); // make sure data is written immediately
        pthread_mutex_unlock(&file_mutex);

        printf("Producer: wrote %d\n", num);

        // Signal that a new value is ready for the consumer
        sem_post(&value_ready);

        // Wait for consumer to read before generating next
        sem_wait(&value_retrieved);
    }

    fclose(fp);
    printf("Producer finished.\n");
    return NULL;
}

// --- Consumer Thread ---
void* consumer(void* arg) {
    (void)arg;
    FILE* fp = fopen(FILENAME, "r");
    if (!fp) {
        perror("Consumer fopen");
        return NULL;
    }

    long read_pos = 0; // tracks where consumer last read
    char line[32];

    printf("Consumer started...\n");

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        // Wait until producer signals a new value
        sem_wait(&value_ready);

        pthread_mutex_lock(&file_mutex);
        fseek(fp, read_pos, SEEK_SET);

        if (fgets(line, sizeof(line), fp)) {
            int val = atoi(line);
            printf("Consumer: read %d\n", val);
            read_pos = ftell(fp);
        }
        pthread_mutex_unlock(&file_mutex);

        // Notify producer that the value was consumed
        sem_post(&value_retrieved);
    }

    fclose(fp);
    printf("Consumer finished.\n");
    return NULL;
}

// --- Main ---
int main(void) {
    srand(time(NULL)); // seed random generator

    pthread_t prod_thread, cons_thread;

    // Initialize synchronization primitives
    pthread_mutex_init(&file_mutex, NULL);
    sem_init(&value_ready, 0, 0);
    sem_init(&value_retrieved, 0, 0);

    // Create producer and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for both to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Cleanup
    sem_destroy(&value_ready);
    sem_destroy(&value_retrieved);
    pthread_mutex_destroy(&file_mutex);

    printf("\nAll %d numbers produced and consumed successfully.\n", TOTAL_ITEMS);
    // remove(FILENAME); // optional cleanup
    return 0;
}