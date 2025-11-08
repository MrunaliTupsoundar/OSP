#include <stdio.h>
#include <limits.h> // For INT_MAX

// Define the Process structure
typedef struct {
    int id;   // Original index/ID
    int at;   // Arrival Time
    int bt;   // Burst Time (Original)
    int rt;   // Remaining Time (for Preemptive)
    int ct;   // Completion Time
    int wt;   // Waiting Time
    int tat;  // Turnaround Time
    int done; // Flag for completion
} P;

// Function to print the scheduling results table
void print_results(P a[], int n, double avg_wt, double avg_tat) {
    printf("\n--- Scheduling Results ---\n");
    printf("+----+-----+-----+-----+-----+-----+\n");
    printf("| ID | AT  | BT  | CT  | TAT | WT  |\n");
    printf("+----+-----+-----+-----+-----+-----+\n");

    // The results array 'a' is passed, but we print based on original ID order if possible,
    // or just the calculated results if the array was modified. 
    // Here we print the results that were calculated and stored in the passed array 'a'.
    for (int i = 0; i < n; i++) {
        printf("| %2d | %3d | %3d | %3d | %3d | %3d |\n",
               a[i].id, a[i].at, a[i].bt, a[i].ct, a[i].tat, a[i].wt);
    }
    printf("+----+-----+-----+-----+-----+-----+\n");
    printf("Average Waiting Time:    %.2f\n", avg_wt);
    printf("Average Turnaround Time: %.2f\n", avg_tat);
    printf("---------------------------\n");
}

// Function to implement FCFS (First-Come, First-Served) - Non-Preemptive
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

        t += a[i].bt;               // Update current time (Completion Time)
        a[i].ct = t;
        a[i].tat = a[i].ct - a[i].at; // Turnaround Time (TAT)
        a[i].wt = a[i].tat - a[i].bt;   // Waiting Time (WT)

        aw += a[i].wt;
        atv += a[i].tat;
    }

    // Print the results
    print_results(a, n, aw / n, atv / n);
}

// Function to implement SJF Preemptive (SRTF - Shortest Remaining Time First)
void srtf_schedule(P a[], int n) {
    double aw = 0, atv = 0;
    int t = 0;      // Current time
    int done = 0;   // Number of completed processes

    // Initialize remaining time and done flag
    for (int i = 0; i < n; i++) {
        a[i].rt = a[i].bt; // Reset Remaining Time
        a[i].done = 0;
    }

    while (done < n) {
        int k = -1;       // Index of the process to run next
        int min_rt = INT_MAX; // Minimum Remaining Time

        // Find the process with the shortest remaining time among the arrived processes
        for (int i = 0; i < n; i++) {
            // Criteria: Not done AND Arrived (AT <= t) AND Shortest Remaining Time (RT < min_rt)
            if (!a[i].done && a[i].at <= t && a[i].rt < min_rt) {
                min_rt = a[i].rt;
                k = i;
            }
        }

        if (k == -1) {
            // No process is ready yet, so advance time (CPU is idle)
            t++;
            continue;
        }

        // Run the selected process (k) for 1 unit of time
        a[k].rt--; // Reduce remaining time
        t++;       // Advance time

        if (a[k].rt == 0) {
            // Process finished
            a[k].done = 1;
            a[k].ct = t; // Completion Time is the current time
            done++;

            // Calculate TAT and WT
            a[k].tat = a[k].ct - a[k].at;
            a[k].wt = a[k].tat - a[k].bt;

            aw += a[k].wt;
            atv += a[k].tat;
        }
    }

    // Print the results
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
    P original_a[n]; // To store a copy of original data

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

    int choice;
    // The Main Continuous Loop
    while (1) { 
        printf("\n--- CPU Scheduling Menu ---\n");
        printf("1. FCFS (First-Come, First-Served)\n");
        printf("2. SJF Preemptive (SRTF - Shortest Remaining Time First)\n");
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
                printf("\n*** Selected: SJF Preemptive (SRTF) ***\n");
                srtf_schedule(a, n);
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