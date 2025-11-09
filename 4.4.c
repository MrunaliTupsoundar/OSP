// readers_writers_reader_priority.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>  // usleep

// --- Configuration ---
#define NUM_READERS 5
#define NUM_WRITERS 2
#define ITERATIONS  5

// --- Shared state ---
int read_count = 0;    // number of readers currently in the DB
int flight_data = 0;   // shared "database" (e.g., seats booked)

// --- Synchronization ---
pthread_mutex_t read_count_mutex; // protects read_count
sem_t db_access;                  // binary semaphore: exclusive DB access

// --- Reader (views flight info) ---
void* reader_activity(void* arg) {
    int id = *(int*)arg;

    for (int k = 0; k < ITERATIONS; k++) {
        // Entry section (reader priority)
        pthread_mutex_lock(&read_count_mutex);
        read_count++;
        if (read_count == 1) {
            // first reader locks DB so writers wait
            sem_wait(&db_access);
        }
        pthread_mutex_unlock(&read_count_mutex);

        // Critical (shared read)
        printf("Reader %d.%d: reading flight data = %d\n", id, k, flight_data);
        usleep(100000); // ~0.1s reading

        // Exit section
        pthread_mutex_lock(&read_count_mutex);
        read_count--;
        if (read_count == 0) {
            // last reader out -> release DB for writers
            sem_post(&db_access);
        }
        pthread_mutex_unlock(&read_count_mutex);

        usleep(50000); // ~0.05s between reads
    }
    return NULL;
}

// --- Writer (makes reservation) ---
void* writer_activity(void* arg) {
    int id = *(int*)arg;

    for (int k = 0; k < ITERATIONS; k++) {
        // Entry: wait for exclusive DB access
        sem_wait(&db_access);

        // Critical (exclusive write)
        flight_data++; // e.g., book one seat
        printf("Writer %d.%d: updated flight data -> %d\n", id, k, flight_data);
        usleep(300000); // ~0.3s writing

        // Exit: release DB
        sem_post(&db_access);

        usleep(100000); // ~0.1s between writes
    }
    return NULL;
}

int main(void) {
    pthread_t rth[NUM_READERS], wth[NUM_WRITERS];
    int rid[NUM_READERS], wid[NUM_WRITERS];

    // Init sync
    if (pthread_mutex_init(&read_count_mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        return 1;
    }
    if (sem_init(&db_access, 0, 1) != 0) {
        perror("sem_init db_access");
        return 1;
    }

    printf("Airline Reservation System (Reader Priority) starting...\n");

    // Create writers first (order doesn’t change correctness)
    for (int i = 0; i < NUM_WRITERS; i++) {
        wid[i] = i + 1;
        pthread_create(&wth[i], NULL, writer_activity, &wid[i]);
    }

    // Create readers
    for (int i = 0; i < NUM_READERS; i++) {
        rid[i] = i + 1;
        pthread_create(&rth[i], NULL, reader_activity, &rid[i]);
    }

    // Join all
    for (int i = 0; i < NUM_READERS; i++) pthread_join(rth[i], NULL);
    for (int i = 0; i < NUM_WRITERS; i++) pthread_join(wth[i], NULL);

    // Cleanup
    sem_destroy(&db_access);
    pthread_mutex_destroy(&read_count_mutex);

    printf("\nSimulation complete. Final flight data: %d\n", flight_data);
    return 0;
}
