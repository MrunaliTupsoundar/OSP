// client.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 5678

int main() {
    int shmid;
    char *shm_addr;

    // 1. Locate the Shared Memory segment
    shmid = shmget(SHM_KEY, 0, 0); // Key only, size 0 means retrieve existing segment
    if (shmid == -1) {
        perror("shmget failed (Is server running?)");
        return 1;
    }
    printf("Client found Shared Memory ID: %d\n", shmid);

    // 2. Attach the segment to the client's data space
    shm_addr = (char *)shmat(shmid, NULL, 0);
    if (shm_addr == (char *)-1) {
        perror("shmat failed");
        return 1;
    }
    printf("Shared Memory attached at address: %p\n", shm_addr);

    // 3. Read and display the message
    printf("\n--- Message from Server ---\n");
    printf("**%s**\n", shm_addr);
    printf("---------------------------\n\n");

    // 4. Detach from the segment
    if (shmdt(shm_addr) == -1) {
        perror("shmdt failed");
        return 1;
    }
    printf("Client detached from Shared Memory. Client exiting.\n");

    // Note: Cleanup (IPC_RMID) is handled by the server.

    return 0;
}