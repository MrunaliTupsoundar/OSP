// sender.c (Simplified)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define KEY 1234
#define SIZE 64

// Message structure
struct msg { 
    long type; 
    char text[SIZE]; 
};

int main() {
    struct msg m;
    long t = 1;

    // Create queue
    int qid = msgget(KEY, IPC_CREAT | 0666);
    if (qid == -1) { perror("msgget"); return 1; }
    printf("Sender running (Queue ID: %d). Type 'quit' to exit.\n", qid);

    while (1) {
        printf("Msg Type %ld > ", t);
        
        // Read input
        if (fgets(m.text, SIZE, stdin) == NULL) break;
        m.text[strcspn(m.text, "\n")] = 0; // Remove newline

        // Termination logic
        if (strcmp(m.text, "quit") == 0) {
            m.type = 999; // Send termination signal
            msgsnd(qid, &m, strlen(m.text) + 1, 0);
            break;
        }

        // Send message
        m.type = t;
        if (msgsnd(qid, &m, strlen(m.text) + 1, 0) == -1) {
            perror("msgsnd"); 
            break;
        }
        t++;
    }

    // Cleanup: Remove the queue
    printf("\nSender removing queue after 2s...\n");
    sleep(2);
    if (msgctl(qid, IPC_RMID, NULL) == -1) {
        perror("msgctl RMID");
        return 1;
    }
    printf("Queue removed. Sender exit.\n");
    return 0;
}