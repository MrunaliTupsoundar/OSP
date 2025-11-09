#include <stdio.h>
#include <limits.h>

// Define the Process structure
typedef struct {
    int id;   // Original index/ID
    int at;   // Arrival Time
    int bt;   // Burst Time (Original)
    int rt;   // Remaining Time (for RR)
    int ct;   // Completion Time
    int wt;   // Waiting Time
    int tat;  // Turnaround Time
    int done; // Flag for completion (will not be heavily used in RR, but kept for consistency)
} P;

// Function to print the scheduling results table
void print_results(P a[], int n, double avg_wt, double avg_tat) {
    printf("\n--- Scheduling Results ---\n");
    printf("+----+-----+-----+-----+-----+-----+\n");
    printf("| ID | AT  | BT  | CT  | TAT | WT  |\n");
    printf("+----+-----+-----+-----+-----+-----+\n");

    for (int i = 0; i < n; i++) {
        printf("| %2d | %3d | %3d | %3d | %3d | %3d |\n",
               a[i].id, a[i].at, a[i].bt, a[i].ct, a[i].tat, a[i].wt);
    }
    printf("+----+-----+-----+-----+-----+-----+\n");
    printf("Average Waiting Time:    %.2f\n", avg_wt);
    printf("Average Turnaround Time: %.2f\n", avg_tat);
    printf("---------------------------\n");
}

// Function to implement FCFS (First-Come, First-Served)
void fcfs_schedule(P a[], int n) {
    double aw = 0, atv = 0;
    int t = 0; // Current time

    // Sort by arrival time (AT).
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (a[j].at < a[i].at) {
                P tP = a[i];
                a[i] = a[j];
                a[j] = tP;
            }
        }
    }

    // Process scheduling
    for (int i = 0; i < n; i++) {
        if (t < a[i].at) {
            t = a[i].at;
        }

        t += a[i].bt;               
        a[i].ct = t;
        a[i].tat = a[i].ct - a[i].at; 
        a[i].wt = a[i].tat - a[i].bt;   

        aw += a[i].wt;
        atv += a[i].tat;
    }

    print_results(a, n, aw / n, atv / n);
}

// Function to implement Round Robin (RR) Scheduling
void rr_schedule(P a[], int n, int quantum) {
    double aw = 0, atv = 0;
    int t = 0;      // Current time
    int done = 0;   // Number of completed processes
    
    // Initialize remaining time
    for (int i = 0; i < n; i++) {
        a[i].rt = a[i].bt; 
    }
    
    // Simple implementation mimicking the original code's logic 
    // by iterating through all processes repeatedly.
    // NOTE: A proper Round Robin implementation usually uses a Queue (or circular array)
    // for the ready processes, but this structure follows the logic of the source code.
    
    // Loop until all processes are done (using a safety limit like the original code's 100000)
    // The loop condition is simplified to just check 'done < n'.
    while (done < n) {
        int moved = 0; // Flag to check if any process ran in this cycle
        
        for (int i = 0; i < n; i++) {
            // Check if process has remaining time AND has arrived
            if (a[i].rt > 0 && a[i].at <= t) {
                // Determine run time: minimum of remaining time or quantum
                int run = (a[i].rt < quantum) ? a[i].rt : quantum; 
                
                a[i].rt -= run;
                t += run;
                moved = 1;

                if (a[i].rt == 0) {
                    // Process finished
                    a[i].ct = t;
                    done++;

                    // Calculate TAT and WT
                    a[i].tat = a[i].ct - a[i].at;
                    a[i].wt = a[i].tat - a[i].bt;
                    
                    aw += a[i].wt;
                    atv += a[i].tat;
                }
            }
        }
        
        // If no process ran in the entire cycle (i.e., CPU was idle or waiting for arrival)
        if (!moved) {
            t++;
            // Safety break: if time reaches an arbitrary large number, stop (optional)
            if (t > 100000) break;
        }
    }

    print_results(a, n, aw / n, atv / n);
}

// Main menu-driven function with continuous loop
int main() {
    int n;
    printf("Enter the number of processes (n): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    P a[n];
    P original_a[n]; 
    int quantum = 2; // Time quantum for Round Robin

    printf("Enter Arrival Time (AT) and Burst Time (BT) for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("P%d (AT BT): ", i);
        if (scanf("%d %d", &a[i].at, &a[i].bt) != 2 || a[i].bt <= 0) {
            printf("Invalid time input. Burst Time must be positive.\n");
            return 1;
        }
        a[i].id = i;
        original_a[i] = a[i]; // Store initial data
    }

    printf("\nNote: Round Robin is implemented with a fixed quantum (q=%d).\n", quantum);

    int choice;
    // The Main Continuous Loop
    while (1) { 
        printf("\n--- CPU Scheduling Menu ---\n");
        printf("1. FCFS (First-Come, First-Served)\n");
        printf("2. Round Robin (q=%d)\n", quantum);
        printf("3. Exit Program\n");
        printf("Enter your choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue; 
        }

        // Restore data from the original array for a clean run
        for (int i = 0; i < n; i++) {
            a[i] = original_a[i];
        }

        switch (choice) {
            case 1:
                printf("\n*** Selected: FCFS ***\n");
                fcfs_schedule(a, n);
                break;
            case 2:
                printf("\n*** Selected: Round Robin (q=%d) ***\n", quantum);
                rr_schedule(a, n, quantum);
                break;
            case 3:
                printf("\nExiting program. Goodbye!\n");
                return 0; // Terminate the program
            default:
                printf("\nInvalid choice (%d). Please select 1, 2, or 3.\n", choice);
                break;
        }
    }
    
    return 0;
}