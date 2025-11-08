#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h> // For usleep

// --- Configuration ---
#define TOTAL_ITEMS 50 // Total number of integers to produce and consume
#define FILENAME "nums.txt"

// --- Synchronization Tools ---

// 1. Mutex: Protects the shared file resource (ensures atomic read/write).
pthread_mutex_t file_mutex; 

// 2. Binary Semaphore 'ready': Producer signals, Consumer waits. (Initial value 0)
//    Indicates: "A new number has been written to the file."
sem_t value_ready;

// 3. Binary Semaphore 'retrieved': Consumer signals, Producer waits. (Initial value 0)
//    Indicates: "The number has been read from the file."
sem_t value_retrieved; 

// Global file position tracker (Necessary for sequential read/write in a shared file)
long current_file_pos = 0;

// --- Producer Function ---
void* producer(void* arg) {
    // Open file in WRITE mode. 'w+' allows read and write, but 'w' truncates; 
    // Since we only write initially, 'w' then 'a' or 'r+' is complex. 
    // A simpler approach is to open 'r+' (read/update) and reset the position tracker.
    FILE* file_ptr = fopen(FILENAME, "w+"); // Open for read/write, truncates the file
    if (file_ptr == NULL) {
        perror("Producer: Error opening file");
        return NULL;
    }
    
    current_file_pos = 0; // Reset file position
    
    printf("Producer started, generating %d numbers...\n", TOTAL_ITEMS);

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        // Simulate production time delay (0 to 100 milliseconds)
        usleep(rand() % 100000);

        int generated_num = rand() % 10;

        // --- START CRITICAL SECTION ---
        // Lock 1: Ensure exclusive access to file manipulation.
        pthread_mutex_lock(&file_mutex); 
        
        // 1. Move file pointer to the end of the previous write
        fseek(file_ptr, current_file_pos, SEEK_SET);

        // 2. Write the new integer to the file.
        fprintf(file_ptr, "%d\n", generated_num);
        fflush(file_ptr); // Force data to be written to disk immediately
        
        // 3. Record the new file position for the consumer's starting point
        current_file_pos = ftell(file_ptr); 

        // Unlock 1
        pthread_mutex_unlock(&file_mutex);
        // --- END CRITICAL SECTION ---

        // SIGNAL 1: Tell the Consumer the value is ready.
        sem_post(&value_ready);

        // WAIT 1: Wait for the Consumer to retrieve the value before producing the next one.
        sem_wait(&value_retrieved);
    }
    
    fclose(file_ptr);
    printf("Producer finished.\n");
    return NULL;
}

// --- Consumer Function ---
void* consumer(void* arg) {
    // Open the same file in READ mode.
    FILE* file_ptr = fopen(FILENAME, "r+"); // Open for read/write
    if (file_ptr == NULL) {
        perror("Consumer: Error opening file");
        return NULL;
    }
    
    int consumed_count = 0;
    int read_num;
    long read_pos = 0; // Local tracker for the consumer's current read position
    
    printf("Consumer started...\n");

    while (consumed_count < TOTAL_ITEMS) {
        
        // WAIT 2: Wait for the Producer to signal that a value is ready.
        sem_wait(&value_ready);

        // --- START CRITICAL SECTION ---
        // Lock 2: Ensure exclusive access to file manipulation.
        pthread_mutex_lock(&file_mutex);

        // 1. Move file pointer to the consumer's last position
        fseek(file_ptr, read_pos, SEEK_SET);

        // 2. Read the integer.
        // %d reads until a non-digit character (like the newline) or EOF
        if (fscanf(file_ptr, "%d\n", &read_num) == 1) { 
            consumed_count++;
            printf("Consumed: %d\n", read_num);
        } else {
             // Handle EOF or read error (shouldn't happen with correct sync)
             pthread_mutex_unlock(&file_mutex);
             continue;
        }

        // 3. Update the consumer's file position to point after the newline
        read_pos = ftell(file_ptr); 

        // Unlock 2
        pthread_mutex_unlock(&file_mutex);
        // --- END CRITICAL SECTION ---

        // SIGNAL 2: Tell the Producer the value has been retrieved.
        sem_post(&value_retrieved);
    }
    
    fclose(file_ptr);
    printf("Consumer finished.\n");
    return NULL;
}

// --- Main Function ---
int main() {
    srand(time(0)); // Seed random number generator
    
    pthread_t prod_thread, cons_thread;

    // 1. Initialize Synchronization Variables:
    if (pthread_mutex_init(&file_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        return 1;
    }
    // value_ready: Binary semaphore, starts at 0 (nothing written yet)
    if (sem_init(&value_ready, 0, 0) != 0) {
        perror("Semaphore 'value_ready' initialization failed");
        return 1;
    }
    // value_retrieved: Binary semaphore, starts at 0 (nothing retrieved yet)
    if (sem_init(&value_retrieved, 0, 0) != 0) {
        perror("Semaphore 'value_retrieved' initialization failed");
        return 1;
    }

    // 2. Create Threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // 3. Wait for Threads to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // 4. Cleanup
    sem_destroy(&value_ready);
    sem_destroy(&value_retrieved);
    pthread_mutex_destroy(&file_mutex);

    printf("\nAll items processed and file closed.\n");

    // Optional: Delete the created file
    // remove(FILENAME); 

    return 0;
}
