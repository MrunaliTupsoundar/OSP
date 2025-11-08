// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 5678
#define SHM_SIZE 1024

int main() {
    int shmid;
    char *shm_addr;
    const char *message = "Hello from the Shared Memory Server!";

    // 1. Create the Shared Memory segment
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }
    printf("Server created Shared Memory ID: %d\n", shmid);

    // 2. Attach the segment to the server's data space
    shm_addr = (char *)shmat(shmid, NULL, 0);
    if (shm_addr == (char *)-1) {
        perror("shmat failed");
        return 1;
    }
    printf("Shared Memory attached at address: %p\n", shm_addr);

    // 3. Write the message to shared memory
    strcpy(shm_addr, message);
    printf("Server wrote: '%s'\n", shm_addr);

    // 4. Synchronization (Wait for Client to read)
    // The server waits, allowing the client process to start, attach, and read the data.
    printf("Server waiting 5 seconds for Client to read...\n");
    sleep(5); 

    // 5. Detach and Cleanup
    if (shmdt(shm_addr) == -1) {
        perror("shmdt failed");
        return 1;
    }
    
    // Remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID failed");
        return 1;
    }
    printf("Shared Memory segment %d removed. Server exiting.\n", shmid);

    return 0;
}
