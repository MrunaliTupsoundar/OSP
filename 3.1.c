#include <stdio.h>
#include <limits.h> // For INT_MAX (used instead of 1e9 for robustness)

// Define the Process structure
typedef struct {
    int id;   // Original index/ID
    int at;   // Arrival Time
    int bt;   // Burst Time
    int ct;   // Completion Time
    int wt;   // Waiting Time
    int tat;  // Turnaround Time
    int done; // Flag for completion in non-preemptive scheduling
} P;

// Function to print the scheduling results table
void print_results(P a[], int n, double avg_wt, double avg_tat) {
    printf("\n--- Scheduling Results ---\n");
    printf("+----+-----+-----+-----+-----+-----+\n");
    printf("| ID | AT  | BT  | CT  | TAT | WT  |\n");
    printf("+----+-----+-----+-----+-----+-----+\n");

    // The processes might have been reordered, so we iterate through them
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

    // Sort by arrival time (AT). This is the FCFS core logic.
    // The original code used a bubble-sort-like swap which is fine for simplicity.
    // We sort the array 'a' in place.
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            // Check for earlier arrival time
            if (a[j].at < a[i].at) {
                P tP = a[i];
                a[i] = a[j];
                a[j] = tP;
            }
            // For a stable sort, a secondary criterion (like ID) can be added,
            // but the basic sort is sufficient for core FCFS.
        }
    }

    // Process scheduling
    for (int i = 0; i < n; i++) {
        // CPU remains idle until the next process arrives
        if (t < a[i].at) {
            t = a[i].at;
        }

        t += a[i].bt;               // Update current time (Completion Time)
        a[i].ct = t;
        a[i].tat = a[i].ct - a[i].at; // Turnaround Time (TAT)
        a[i].wt = a[i].tat - a[i].bt;   // Waiting Time (WT)

        aw += a[i].wt;
        atv += a[i].tat;
    }

    // Print the results using the dedicated function
    print_results(a, n, aw / n, atv / n);
}

// Function to implement SJF (Shortest Job First) non-preemptive
void sjf_schedule(P a[], int n) {
    double aw = 0, atv = 0;
    int t = 0;      // Current time
    int done = 0;   // Number of completed processes

    // Reset the done flag for safety, as the same array might be reused (though not in this minimal design)
    for (int i = 0; i < n; i++) {
        a[i].done = 0;
    }

    while (done < n) {
        int k = -1; // Index of the process to run next
        int mb = INT_MAX; // Minimum Burst time

        // Find the process with the shortest burst time
        for (int i = 0; i < n; i++) {
            // Criteria: Not done AND Arrived (AT <= t) AND Shortest Burst Time (BT < mb)
            if (!a[i].done && a[i].at <= t && a[i].bt < mb) {
                mb = a[i].bt;
                k = i;
            }
        }

        if (k == -1) {
            // No process is ready yet, so advance time (CPU is idle)
            t++;
            continue;
        }

        // Run the selected process (k)
        t += a[k].bt;
        a[k].ct = t;
        a[k].tat = t - a[k].at;
        a[k].wt = a[k].tat - a[k].bt;
        a[k].done = 1;
        done++;

        aw += a[k].wt;
        atv += a[k].tat;
    }

    // Print the results using the dedicated function
    print_results(a, n, aw / n, atv / n);
}

// Main menu-driven function
int main() {
    int n;
    printf("Enter the number of processes (n): ");
    scanf("%d", &n);

    P a[n];
    P original_a[n]; // To store a copy of original data

    printf("Enter Arrival Time (AT) and Burst Time (BT) for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("P%d (AT BT): ", i);
        a[i].id = i;
        scanf("%d %d", &a[i].at, &a[i].bt);
        a[i].done = 0;
        original_a[i] = a[i]; // Store initial data as it's entered
    }

    int choice;
    // --- The Main Continuous Loop ---
    while (1) { // Loop indefinitely until 'break' (choice 3)
        printf("\n--- CPU Scheduling Menu ---\n");
        printf("1. FCFS (First-Come, First-Served)\n");
        printf("2. SJF (Shortest Job First) Non-Preemptive\n");
        printf("3. Exit Program\n");
        printf("Enter your choice (1-3): ");
        
        // Check if scanf successfully read an integer
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            // Clear input buffer to prevent infinite loop on non-integer input
            while (getchar() != '\n');
            continue; 
        }

        // Restore data from the original array before running the schedule
        // This is crucial to ensure each algorithm runs on the *original* process data.
        for (int i = 0; i < n; i++) {
            a[i] = original_a[i];
        }

        switch (choice) {
            case 1:
                printf("\n*** Selected: FCFS ***\n");
                fcfs_schedule(a, n);
                break;
            case 2:
                printf("\n*** Selected: SJF Non-Preemptive ***\n");
                sjf_schedule(a, n);
                break;
            case 3:
                printf("\nExiting program. Goodbye!\n");
                return 0; // Terminate the program
            default:
                printf("\nInvalid choice (%d). Please select 1, 2, or 3.\n", choice);
                break;
        }
    }
    // --- End of Continuous Loop ---
    
    return 0;
}