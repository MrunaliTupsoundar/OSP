// 5_2.c - Compact Banker's Algorithm Simulation
#include <stdio.h>
#include <stdlib.h>

// Function to run the Safety Check
// Returns 1 (safe) or 0 (unsafe)
int check_safety(int n, int m, int A[n][m], int M[n][m], int Av[m]) {
    int Work[m], Finish[n], i, j;
    
    // Copy Available to Work and init Finish
    for (j = 0; j < m; j++) Work[j] = Av[j];
    for (i = 0; i < n; i++) Finish[i] = 0;

    int done = 0;
    while (done < n) {
        int progress = 0;
        for (i = 0; i < n; i++) {
            if (!Finish[i]) {
                int ok = 1;
                // Check if Need <= Work
                for (j = 0; j < m; j++) {
                    if (M[i][j] - A[i][j] > Work[j]) {
                        ok = 0;
                        break;
                    }
                }
                
                if (ok) {
                    // Release resources: Work = Work + Allocation
                    for (j = 0; j < m; j++) Work[j] += A[i][j];
                    Finish[i] = 1;
                    progress = 1;
                    done++;
                }
            }
        }
        if (!progress) break; // Deadlock/Unsafe state detected
    }
    return (done == n); // Return 1 if all processes finished, 0 otherwise
}

int main() {
    FILE* f = fopen("state.txt", "r");
    if (f == NULL) {
        puts("Error: state.txt not found.");
        return 1;
    }

    int n, m;
    // Read N and M (Number of Processes, Resources)
    if (fscanf(f, "%d %d", &n, &m) != 2) {
        puts("Error reading N and M.");
        return 1;
    }

    int A[n][m], M[n][m], Av[m];

    // Read Allocation Matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            fscanf(f, "%d", &A[i][j]);

    // Read Max Matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            fscanf(f, "%d", &M[i][j]);

    // Read Available Vector
    for (int j = 0; j < m; j++)
        fscanf(f, "%d", &Av[j]);

    fclose(f);

    // --- Resource Request Input ---
    int p; // Process ID making the request
    printf("Enter Process ID and Resource Request (e.g., 1 0 1 0): \n");
    if (scanf("%d", &p) != 1) return 1;

    int R[m]; // Request vector
    for (int j = 0; j < m; j++)
        if (scanf("%d", &R[j]) != 1) return 1;

    // --- Temporary State Backup ---
    int T_Av[m]; // Temp Available
    int T_A[n][m]; // Temp Allocation
    for (int j = 0; j < m; j++) T_Av[j] = Av[j];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) T_A[i][j] = A[i][j];

    // --- 1. Request Feasibility Check ---
    for (int j = 0; j < m; j++) {
        // Check 1: Request <= Need
        if (R[j] > (M[p][j] - A[p][j])) {
            puts("DENY (Request exceeds Need)");
            return 0;
        }
        // Check 2: Request <= Available (Note: If this fails, it's a 'WAIT', not DENY in Banker's)
        if (R[j] > Av[j]) {
            puts("DENY (Request exceeds Available - Must Wait)");
            return 0; 
        }
    }
    
    // --- 2. Tentatively Grant Request on Temporary State ---
    for (int j = 0; j < m; j++) {
        T_Av[j] -= R[j];
        T_A[p][j] += R[j];
    }

    // --- 3. Run Safety Algorithm on Temporary State ---
    if (check_safety(n, m, T_A, M, T_Av)) {
        puts("GRANT");
    } else {
        puts("DENY (Unsafe State)");
    }

    return 0;
}