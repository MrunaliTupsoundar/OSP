#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // For usleep

// --- Configuration ---
#define BUFFER_SIZE 5       // N: Max capacity of the Pizza Counter
#define ITEMS_TO_PROCESS 20 // Total number of pizzas to be baked and consumed

// --- Shared Resources ---
int pizza_counter[BUFFER_SIZE]; // The circular buffer (Pizza counter)
int in = 0;                     // Index where Producer (Simpson) inserts the next item
int out = 0;                    // Index where Consumer (Joey) extracts the next item

// --- Synchronization Tools ---

// 1. Mutex (Binary Semaphore / Lock): Ensures only one thread can access the shared buffer at a time.
pthread_mutex_t counter_mutex;

// 2. Counting Semaphore 'empty': Counts the number of empty slots available.
sem_t empty_slots;

// 3. Counting Semaphore 'full': Counts the number of full slots (pizzas available).
sem_t full_slots;

// --- Producer Function: Mr. Simpson (Bakes Pizza) ---
void* mr_simpson_baker(void* arg) {
    for (int pizza_num = 1; pizza_num <= ITEMS_TO_PROCESS; pizza_num++) {

        // WAIT 1: Flow Control - Wait if the counter is full (empty_slots == 0).
        // Simpson waits until Joey signals that a slot is empty.
        sem_wait(&empty_slots);

        // --- START CRITICAL SECTION ---

        // LOCK 1: Mutual Exclusion - Acquire the lock to safely access shared resources (buffer, in).
        pthread_mutex_lock(&counter_mutex);

        // 2. Produce item (Place pizza on counter)
        pizza_counter[in] = pizza_num;
        printf("Producer (Simpson) baked: %d\n", pizza_num);
        in = (in + 1) % BUFFER_SIZE; // Circular buffer update

        // UNLOCK 1: Release the lock.
        pthread_mutex_unlock(&counter_mutex);

        // --- END CRITICAL SECTION ---

        // SIGNAL 1: Flow Control - Signal that a slot is now full.
        sem_post(&full_slots);

        usleep(300000); // Simulate baking time (0.3s)
    }
    return NULL;
}

// --- Consumer Function: Joey Tribbiani (Consumes Pizza) ---
void* joey_tribbiani_eater(void* arg) {
    int consumed_pizza;
    for (int i = 0; i < ITEMS_TO_PROCESS; i++) {

        // WAIT 2: Flow Control - Wait if the counter is empty (full_slots == 0).
        // Joey waits until Simpson signals that a pizza is available.
        sem_wait(&full_slots);

        // --- START CRITICAL SECTION ---

        // LOCK 2: Mutual Exclusion - Acquire the lock to safely access shared resources (buffer, out).
        pthread_mutex_lock(&counter_mutex);

        // 2. Consume item (Take pizza from counter)
        consumed_pizza = pizza_counter[out];
        printf("Consumer (Joey) consumed: %d\n", consumed_pizza);
        out = (out + 1) % BUFFER_SIZE; // Circular buffer update

        // UNLOCK 2: Release the lock.
        pthread_mutex_unlock(&counter_mutex);

        // --- END CRITICAL SECTION ---

        // SIGNAL 2: Flow Control - Signal that a slot is now empty.
        sem_post(&empty_slots);

        usleep(500000); // Simulate eating time (0.5s)
    }
    return NULL;
}

// --- Main Function ---
int main() {
    pthread_t producer_thread, consumer_thread;

    // 1. Initialize Synchronization Variables:
    // Initialize mutex (simplest way)
    if (pthread_mutex_init(&counter_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        return 1;
    }

    // Initialize 'empty_slots': Initial count = BUFFER_SIZE (all 5 slots are empty).
    if (sem_init(&empty_slots, 0, BUFFER_SIZE) != 0) {
        perror("Semaphore 'empty_slots' initialization failed");
        return 1;
    }

    // Initialize 'full_slots': Initial count = 0 (no slots are full).
    if (sem_init(&full_slots, 0, 0) != 0) {
        perror("Semaphore 'full_slots' initialization failed");
        return 1;
    }

    // 2. Create Threads
    printf("Starting Producer (Simpson) and Consumer (Joey)...\n");
    pthread_create(&producer_thread, NULL, mr_simpson_baker, NULL);
    pthread_create(&consumer_thread, NULL, joey_tribbiani_eater, NULL);

    // 3. Wait for Threads to finish (i.e., until ITEMS_TO_PROCESS are done)
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // 4. Cleanup
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&counter_mutex);

    printf("\nAll %d pizzas have been baked and consumed. Synchronization successful!\n", ITEMS_TO_PROCESS);

    return 0;
}