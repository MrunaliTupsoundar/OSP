#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // For usleep

// --- Configuration ---
#define CHAIRS 3 // The number of chairs in the hallway
#define TOTAL_STUDENTS 15 // Number of students who will attempt to seek help

// --- Shared State Variables ---
int waiting_students = 0; // The number of students currently sitting in the hallway
pthread_mutex_t access_chairs; // Mutex lock to protect the shared 'waiting_students' count
int student_id_counter = 0; // Simple counter for student IDs

// --- Synchronization Semaphores ---
// 1. students: Counting semaphore. Signaled by a student to wake the TA.
//    Initial value 0 (TA starts sleeping, 0 students are waiting).
sem_t students; 

// 2. ta_ready: Counting semaphore. Signaled by the TA to let one student enter for help.
//    Initial value 0 (TA is not ready to help anyone yet).
sem_t ta_ready; 


// --- TA (Teaching Assistant) Function ---
void* ta_activity(void* arg) {
    printf("TA: Office hours started. No students, I'm taking a nap...\n");
    
    // The TA's loop runs indefinitely during office hours.
    for (;;) {
        // 1. Wait for a student to arrive (or wake me up)
        // If 'students' is 0, the TA sleeps until a student calls sem_post(&students).
        sem_wait(&students); 

        // 2. Acquire lock to safely update the shared waiting count.
        pthread_mutex_lock(&access_chairs);
        
        waiting_students--; // A waiting student is now entering the office.
        
        // TA is now busy helping the student.
        printf("TA: Student is now entering. Waiting students left: %d\n", waiting_students);
        
        // 3. Release lock before signaling the student.
        pthread_mutex_unlock(&access_chairs);

        // 4. Signal the waiting student to enter the office.
        // The student who called sem_post(&students) now waits on this signal.
        sem_post(&ta_ready);

        // 5. Simulate helping the student (Critical section duration)
        printf("TA: Currently helping student...\n");
        usleep(1500000); // Takes 1.5 seconds to help one student

        printf("TA: Finished helping the student.\n");
    }
    return NULL;
}

// --- Student Function ---
void* student_activity(void* arg) {
    int student_id = *((int*)arg);
    printf("Student %d: I arrived for help.\n", student_id);
    
    // 1. Acquire lock to check the number of waiting chairs.
    pthread_mutex_lock(&access_chairs);

    if (waiting_students < CHAIRS) {
        // --- Student Waits ---
        waiting_students++; // Take a seat in the hallway
        
        printf("Student %d: TA is busy, I'm taking a seat. Waiting: %d/%d\n", student_id, waiting_students, CHAIRS);
        
        // 2. Signal the TA (If the TA was sleeping, this wakes them up).
        // The TA will now proceed from sem_wait(&students).
        sem_post(&students);

        // 3. Release lock (Done updating shared variables).
        pthread_mutex_unlock(&access_chairs);

        // 4. Wait for the TA to be ready to help this student.
        // The student waits until the TA calls sem_post(&ta_ready).
        sem_wait(&ta_ready);

        // 5. TA is now helping.
        printf("Student %d: TA is helping me now. (Problem solved!)\n", student_id);
        
    } else {
        // --- Student Leaves ---
        
        // 2. Release lock immediately since no shared state was changed.
        pthread_mutex_unlock(&access_chairs);
        
        // 3. The student leaves (No available chairs).
        printf("Student %d: No chairs available, I will come back later.\n", student_id);
    }

    return NULL;
}

// --- Main Function ---
int main() {
    pthread_t ta_thread;
    pthread_t student_threads[TOTAL_STUDENTS];

    // Initialize synchronization variables
    if (pthread_mutex_init(&access_chairs, NULL) != 0) {
        perror("Mutex initialization failed");
        return 1;
    }
    // 'students' semaphore starts at 0 (no one waiting, TA sleeps)
    if (sem_init(&students, 0, 0) != 0) {
        perror("Semaphore 'students' initialization failed");
        return 1;
    }
    // 'ta_ready' semaphore starts at 0 (TA is not ready to start helping)
    if (sem_init(&ta_ready, 0, 0) != 0) {
        perror("Semaphore 'ta_ready' initialization failed");
        return 1;
    }
    
    // Create TA thread
    pthread_create(&ta_thread, NULL, ta_activity, NULL);

    // Create student threads with a short delay to simulate staggered arrival
    printf("--- Simulating %d students arriving ---\n", TOTAL_STUDENTS);
    int *student_ids[TOTAL_STUDENTS];
    for (int i = 0; i < TOTAL_STUDENTS; i++) {
        // Allocate memory for the student ID to pass to the thread
        student_ids[i] = (int*)malloc(sizeof(int));
        if (student_ids[i] == NULL) {
            perror("Failed to allocate student ID");
            exit(EXIT_FAILURE);
        }
        *student_ids[i] = i + 1; // Assign ID
        
        pthread_create(&student_threads[i], NULL, student_activity, student_ids[i]);
        usleep(rand() % 500000 + 100000); // Random arrival delay between 0.1s and 0.6s
    }

    // Wait for all student threads to finish
    for (int i = 0; i < TOTAL_STUDENTS; i++) {
        pthread_join(student_threads[i], NULL);
        free(student_ids[i]); // Clean up allocated memory
    }

    // Since the TA loop is infinite, we can't join it easily. 
    // For a real-world scenario, you'd signal the TA thread to exit.
    // For this simulation, we'll just detach it and let the main thread exit.
    pthread_detach(ta_thread);

    // Cleanup synchronization variables
    sem_destroy(&students);
    sem_destroy(&ta_ready);
    pthread_mutex_destroy(&access_chairs);

    printf("\n--- All student arrivals processed. ---\n");
    return 0;
}