#include <stdio.h>
#include <stdbool.h> // For using bool type for Finished array

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

int main() {
    int n, m, i, j, k;

    // --- 1. Input Section ---
    printf("Enter the number of processes (max %d): ", MAX_PROCESSES);
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROCESSES) {
        printf("Invalid number of processes.\n");
        return 1;
    }
    printf("Enter the number of resources (max %d): ", MAX_RESOURCES);
    if (scanf("%d", &m) != 1 || m <= 0 || m > MAX_RESOURCES) {
        printf("Invalid number of resources.\n");
        return 1;
    }

    int max[n][m], allocation[n][m], available[m], need[n][m];
    int work[m];
    bool finish[n]; // Use bool for clarity: true/false
    int safeSequence[n];

    // Get input for Max, Allocation, and Available
    printf("\n--- Input Data ---\n");
    
    // Get the maximum resource demand for each process
    printf("Enter the Max Matrix:\n");
    for (i = 0; i < n; i++) {
        printf("For Process P%d (%d resources): ", i, m);
        for (j = 0; j < m; j++) {
            scanf("%d", &max[i][j]);
        }
    }

    // Get the resources currently allocated to each process
    printf("Enter the Allocation Matrix:\n");
    for (i = 0; i < n; i++) {
        printf("For Process P%d (%d resources): ", i, m);
        for (j = 0; j < m; j++) {
            scanf("%d", &allocation[i][j]);
        }
    }

    // Get the number of available resources
    printf("Enter the Available Resources (%d resources): ", m);
    for (i = 0; i < m; i++) {
        scanf("%d", &available[i]);
    }

    // --- 2. Initialization and Need Calculation ---

    // Calculate the Need matrix: Need = Max - Allocation
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
            if (need[i][j] < 0) {
                printf("Error: Allocation exceeds Max demand for P%d, resource %d.\n", i, j);
                return 1;
            }
        }
        finish[i] = false; // Initialize all processes as NOT finished
    }

    // Initialize Work vector: Work = Available
    for (i = 0; i < m; i++) {
        work[i] = available[i];
    }
    
    printf("\n--- Safety Algorithm Execution ---\n");
    
    // --- 3. Safety Algorithm ---

    int count = 0; // Number of processes successfully completed
    int found_in_pass; // Flag to check if any process completed in a pass

    // Outer loop: Continues until all processes finish or no more progress can be made
    do {
        found_in_pass = 0;

        // Inner loop: Check every process
        for (i = 0; i < n; i++) {
            if (finish[i] == false) { // Only consider non-finished processes
                int canExecute = 1;
                
                // Check if Need[i] <= Work
                for (j = 0; j < m; j++) {
                    if (need[i][j] > work[j]) {
                        canExecute = 0;
                        break; // Move to the next process
                    }
                }

                if (canExecute) {
                    // Process P[i] can execute.
                    
                    // Work = Work + Allocation[i] (Release resources)
                    for (j = 0; j < m; j++) {
                        work[j] += allocation[i][j];
                    }
                    
                    safeSequence[count++] = i;
                    finish[i] = true; // Mark as finished
                    found_in_pass = 1;

                    printf("Process P%d can finish (Need <= Work). New Work: [", i);
                    for(j=0; j<m; j++) printf("%d%s", work[j], j==m-1 ? "" : ", ");
                    printf("]\n");
                }
            }
        }

    } while (found_in_pass != 0 && count < n);


    // --- 4. Result and Conclusion ---

    printf("\n--- Conclusion ---\n");
    if (count == n) {
        // Safe State
        printf("The system is in a **SAFE STATE**.\n");
        printf("Safe Sequence is: ");
        for (i = 0; i < n; i++) {
            printf("P%d", safeSequence[i]);
            if (i < n - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
    } else {
        // Unsafe State
        printf("The system is in an **UNSAFE STATE**.\n");
        printf("The process(es) that could not complete are: ");
        int first_one = 1;
        for (i = 0; i < n; i++) {
            if (finish[i] == false) {
                if (!first_one) printf(", ");
                printf("P%d", i);
                first_one = 0;
            }
        }
        printf("\nThis is because their resource need is greater than the current available resources (Work).\n");
    }

    return 0;
}