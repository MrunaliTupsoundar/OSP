// sleeping_ta_pc.c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>   // usleep

// --- Configuration ---
#define CHAIRS 3
#define TOTAL_STUDENTS 15

// --- Shared State ---
int waiting = 0;                 // number of students waiting (0..CHAIRS)

// --- Synchronization ---
pthread_mutex_t chair_mutex;     // protects 'waiting'
sem_t students_sem;              // counts waiting students (wake TA / queue length)
sem_t ta_ready_sem;              // TA signals exactly one student to enter

// --- TA (consumer) ---
void* ta_thread(void* arg) {
    puts("TA: office open; napping until a student arrives...");
    for (;;) {
        // Wait until at least one student is waiting
        sem_wait(&students_sem);

        // One student leaves the hallway to enter the office
        pthread_mutex_lock(&chair_mutex);
        waiting--;
        printf("TA: calling next student. waiting now: %d/%d\n", waiting, CHAIRS);
        pthread_mutex_unlock(&chair_mutex);

        // Let exactly one student enter
        sem_post(&ta_ready_sem);

        // Help the student
        puts("TA: helping a student...");
        usleep(1500000); // ~1.5s
        puts("TA: done.");
    }
    return NULL;
}

// --- Student (producer) ---
void* student_thread(void* arg) {
    int id = *(int*)arg;

    // Try to take a chair atomically
    pthread_mutex_lock(&chair_mutex);
    if (waiting < CHAIRS) {
        waiting++;  // take a seat in hallway
        printf("Student %d: seated. waiting %d/%d\n", id, waiting, CHAIRS);

        // Announce / wake TA that a student is waiting
        sem_post(&students_sem);
        pthread_mutex_unlock(&chair_mutex);

        // Wait until TA invites me in
        sem_wait(&ta_ready_sem);
        printf("Student %d: getting help now.\n", id);
        // (TA simulates help time; student just returns.)
    } else {
        pthread_mutex_unlock(&chair_mutex);
        printf("Student %d: no chairs free, leaving.\n", id);
    }
    return NULL;
}

int main() {
    pthread_t ta;
    pthread_t students[TOTAL_STUDENTS];
    int ids[TOTAL_STUDENTS];

    // Init sync primitives
    pthread_mutex_init(&chair_mutex, NULL);
    sem_init(&students_sem, 0, 0);   // no one waiting initially
    sem_init(&ta_ready_sem, 0, 0);   // TA hasn't invited anyone yet

    // Start TA
    pthread_create(&ta, NULL, ta_thread, NULL);

    // Spawn students with small stagger
    puts("--- spawning students ---");
    for (int i = 0; i < TOTAL_STUDENTS; i++) {
        ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_thread, &ids[i]);
        usleep(200000); // ~0.2s between arrivals (tweak as you like)
    }

    // Join students
    for (int i = 0; i < TOTAL_STUDENTS; i++)
        pthread_join(students[i], NULL);

    // Keep demo simple: TA runs indefinitely (like continuous office hours)
    pthread_detach(ta);

    // Cleanup
    sem_destroy(&students_sem);
    sem_destroy(&ta_ready_sem);
    pthread_mutex_destroy(&chair_mutex);

    puts("--- all students processed (helped or left). ---");
    return 0;
}
