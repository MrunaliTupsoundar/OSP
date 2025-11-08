#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // For usleep and thread simulation delays

// --- Configuration ---
#define NUM_READERS 5 // Number of threads reading flight information
#define NUM_WRITERS 2 // Number of threads making reservations
#define ITERATIONS 5  // Each thread performs 5 actions

// --- Shared State Variables ---
int read_count = 0; // Number of threads currently reading the data
int flight_data = 0; // The shared data (e.g., number of seats booked)

// --- Synchronization Tools ---

// 1. Mutex: Protects the shared variable 'read_count'.
pthread_mutex_t read_count_mutex; 

// 2. Binary Semaphore 'db_access': Controls exclusive access to the 'flight_data' (the database).
//    Initial value 1 (Allows one Writer OR the first Reader group to enter).
sem_t db_access; 

// --- Reader Function (Views Flight Information) ---
void* reader_activity(void* arg) {
    long reader_id = (long)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        // --- Reader Entry Section ---
        
        // LOCK 1: Acquire lock to safely modify 'read_count'.
        pthread_mutex_lock(&read_count_mutex);
        
        read_count++; // Announce arrival
        
        // If this is the FIRST reader, acquire the database lock.
        // This blocks any waiting writers, prioritizing the incoming readers.
        if (read_count == 1) {
            sem_wait(&db_access); // Waits if a Writer is currently holding the lock.
        }
        
        // UNLOCK 1: Release the lock quickly.
        pthread_mutex_unlock(&read_count_mutex);

        // --- Reading (Non-Critical) Section ---
        printf("Reader %ld: Reading flight data. Current value: %d\n", reader_id, flight_data);
        usleep(100000); // Simulate time spent reading (0.1s)

        // --- Reader Exit Section ---
        
        // LOCK 2: Acquire lock to safely modify 'read_count'.
        pthread_mutex_lock(&read_count_mutex);
        
        read_count--; // Announce departure
        
        // If this is the LAST reader, release the database lock.
        // This allows a waiting Writer to proceed.
        if (read_count == 0) {
            sem_post(&db_access);
        }
        
        // UNLOCK 2: Release the lock.
        pthread_mutex_unlock(&read_count_mutex);

        usleep(50000); // Simulate delay before reading again
    }
    return NULL;
}

// --- Writer Function (Makes Flight Reservations) ---
void* writer_activity(void* arg) {
    long writer_id = (long)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        
        // --- Writer Entry Section ---
        // Wait until all Readers and any other Writer have released the lock.
        // If readers are holding 'db_access', this Writer is blocked here.
        sem_wait(&db_access); 
        
        // --- Writing (Critical) Section ---
        // Access is exclusive—no other Writers or Readers are active.
        flight_data++; // Make a reservation (update the data)
        printf("Writer %ld: Updated flight data. New value: %d\n", writer_id, flight_data);
        usleep(300000); // Simulate time spent writing (0.3s)
        
        // --- Writer Exit Section ---
        sem_post(&db_access); // Release the lock, allowing the next Reader or Writer to proceed.

        usleep(100000); // Simulate delay before writing again
    }
    return NULL;
}

// --- Main Function ---
int main() {
    pthread_t reader_threads[NUM_READERS];
    pthread_t writer_threads[NUM_WRITERS];
    
    // 1. Initialize Synchronization Variables:
    // read_count_mutex: Initialized normally to protect 'read_count'.
    if (pthread_mutex_init(&read_count_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        return 1;
    }
    // db_access: Binary semaphore, starts at 1 (database is initially available).
    if (sem_init(&db_access, 0, 1) != 0) {
        perror("Semaphore initialization failed");
        return 1;
    }

    printf("Starting Airline Reservation System Simulation (Reader Priority)...\n");

    // 2. Create Writer threads
    for (long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writer_threads[i], NULL, writer_activity, (void*)(i + 1));
    }
    
    // 3. Create Reader threads
    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&reader_threads[i], NULL, reader_activity, (void*)(i + 1));
    }

    // 4. Wait for all threads to finish
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(reader_threads[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writer_threads[i], NULL);
    }

    // 5. Cleanup
    sem_destroy(&db_access);
    pthread_mutex_destroy(&read_count_mutex);

    printf("\nSimulation complete. Final flight data value: %d\n", flight_data);

    return 0;
}