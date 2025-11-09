// receiver.c (Simplified)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 1234
#define SIZE 64

// Message structure (must match sender)
struct msg { 
    long type; 
    char text[SIZE]; 
};

int main() {
    struct msg m;

    // Connect to the queue
    int qid = msgget(KEY, 0);
    if (qid == -1) { 
        perror("msgget (Is sender running?)"); 
        return 1; 
    }
    printf("Receiver running (Connected to ID: %d). Waiting for messages...\n", qid);

    // Read loop (type 0 reads all messages)
    while (msgrcv(qid, &m, sizeof(m.text), 0, 0) > 0) {
        
        // Check for termination signal
        if (m.type == 999) {
            printf("\nReceiver received termination signal. Exit.\n");
            break;
        }

        // Convert to uppercase
        for (int i = 0; m.text[i]; i++) {
            m.text[i] = toupper((unsigned char)m.text[i]);
        }
        
        // Display result
        printf("RCV Type %ld: %s\n", m.type, m.text);
    }
    
    // Check if loop ended because queue was removed
    if (msgctl(qid, IPC_STAT, NULL) == -1) {
        printf("Queue deleted by sender. Receiver exit.\n");
    }

    return 0;
}
