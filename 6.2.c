#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // Required for INT_MAX

// Define the fixed page reference string and its length
const int FIXED_REF_STRING[] = {1, 2, 3, 4, 1, 2, 5, 1, 1, 2, 3, 4, 5};
#define REF_STRING_LEN 13
#define MAX_FRAMES 4 // Max frame size used in this simulation

// Helper function to print the current state of the frames
void print_frames(const int frame_array[], int frames, int page_fault) {
    for (int j = 0; j < frames; j++) {
        // Print -1 as '_' for a cleaner output
        if (frame_array[j] == -1) {
            printf("_ ");
        } else {
            printf("%d ", frame_array[j]);
        }
    }
    printf("| %s\n", (page_fault ? "YES" : "NO"));
}

// -------------------------------------------------------------------
// 1. Optimal Simulation
// -------------------------------------------------------------------
void simulate_optimal(const int ref_string[], int pages, int frames) {
    int frame_array[MAX_FRAMES];
    for (int i = 0; i < frames; i++) {
        frame_array[i] = -1;
    }

    int page_faults = 0;

    printf("\n\n--- OPTIMAL Algorithm Simulation (Frames: %d) ---\n", frames);
    printf("Page Reference | Frames | Page Fault\n");
    printf("--------------------------------------------\n");

    for (int i = 0; i < pages; i++) {
        int page = ref_string[i];
        int page_found = 0;
        
        printf("       %d         | ", page);

        // 1. Check for Page Hit
        for (int j = 0; j < frames; j++) {
            if (frame_array[j] == page) {
                page_found = 1;
                break;
            }
        }

        if (page_found == 0) {
            // Page Fault
            page_faults++;
            int victim_frame_index = -1;

            // 2. Check for an empty frame
            for (int j = 0; j < frames; j++) {
                if (frame_array[j] == -1) {
                    victim_frame_index = j;
                    break;
                }
            }

            // 3. Find the Optimal Victim (if no empty frame)
            if (victim_frame_index == -1) {
                int max_future_distance = -1;

                for (int j = 0; j < frames; j++) {
                    int current_page_in_frame = frame_array[j];
                    int next_occurrence_index = pages; // Default to 'never occurs again'

                    // Look ahead starting from the next reference (i + 1)
                    for (int k = i + 1; k < pages; k++) {
                        if (ref_string[k] == current_page_in_frame) {
                            next_occurrence_index = k; 
                            break; 
                        }
                    }
                    
                    // The page that is used furthest in the future (largest index) is the victim
                    if (next_occurrence_index > max_future_distance) {
                        max_future_distance = next_occurrence_index;
                        victim_frame_index = j;
                    }
                }
            }

            // Replace the victim
            frame_array[victim_frame_index] = page;
            print_frames(frame_array, frames, 1);
        } else {
            // Page Hit
            print_frames(frame_array, frames, 0);
        }
    }

    printf("--------------------------------------------\n");
    printf("Total Page Faults (Optimal): %d\n", page_faults);
    printf("Total Page Hits (Optimal):   %d\n", pages - page_faults);
    printf("Hit Ratio (Optimal):         %.2f%%\n", (float)(pages - page_faults) * 100 / pages);
    printf("Miss Ratio (Optimal):        %.2f%%\n", (float)page_faults * 100 / pages);
}

// -------------------------------------------------------------------
// 2. LRU Simulation
// -------------------------------------------------------------------
void simulate_lru(const int ref_string[], int pages, int frames) {
    int frame_array[MAX_FRAMES];
    // To store the last access time (index in the reference string) for each frame
    int last_used_time[MAX_FRAMES]; 
    
    for (int i = 0; i < frames; i++) {
        frame_array[i] = -1;
        last_used_time[i] = 0;
    }

    int page_faults = 0;

    printf("\n\n--- LRU Algorithm Simulation (Frames: %d) ---\n", frames);
    printf("Page Reference | Frames | Page Fault\n");
    printf("--------------------------------------------\n");

    for (int i = 0; i < pages; i++) {
        int page = ref_string[i];
        int page_found = 0;
        
        printf("       %d         | ", page);

        // Check for Page Hit and update time if hit
        for (int j = 0; j < frames; j++) {
            if (frame_array[j] == page) {
                page_found = 1;
                last_used_time[j] = i; // Update the last used time to current reference index
                break;
            }
        }

        if (page_found == 0) {
            // Page Fault
            page_faults++;
            int lru_index = 0;
            int min_time = INT_MAX;

            // Find the least recently used page to replace
            for (int j = 0; j < frames; j++) {
                if (frame_array[j] == -1) { // Priority 1: Find an empty frame
                    lru_index = j;
                    break;
                }
                if (last_used_time[j] < min_time) {
                    min_time = last_used_time[j];
                    lru_index = j;
                }
            }

            // Replace the LRU page (or fill the empty slot)
            frame_array[lru_index] = page;
            last_used_time[lru_index] = i; // Update time for the newly inserted page

            print_frames(frame_array, frames, 1);
        } else {
            // Page Hit
            print_frames(frame_array, frames, 0);
        }
    }

    printf("--------------------------------------------\n");
    printf("Total Page Faults (LRU): %d\n", page_faults);
    printf("Total Page Hits (LRU):   %d\n", pages - page_faults);
    printf("Hit Ratio (LRU):         %.2f%%\n", (float)(pages - page_faults) * 100 / pages);
    printf("Miss Ratio (LRU):        %.2f%%\n", (float)page_faults * 100 / pages);
}

// -------------------------------------------------------------------
// 3. Main Program
// -------------------------------------------------------------------
int main() {
    int pages = REF_STRING_LEN;
    const int *ref_string = FIXED_REF_STRING;

    printf("--- Page Replacement Simulation: Optimal vs. LRU ---\n");
    printf("Reference String: ");
    for(int i = 0; i < pages; i++) {
        printf("%d%s", ref_string[i], (i == pages - 1 ? "" : ", "));
    }
    printf("\nReference Length: %d pages\n", pages);
    
    // Simulation for Frame Size 3
    simulate_optimal(ref_string, pages, 3);
    simulate_lru(ref_string, pages, 3);

    // Simulation for Frame Size 4
    simulate_optimal(ref_string, pages, 4);
    simulate_lru(ref_string, pages, 4);

    return 0;
}