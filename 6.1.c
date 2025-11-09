#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // Required for INT_MAX for LRU

// Constants for array size limits
#define MAX_REF_LEN 100
#define MAX_FRAMES 10 
#define MAX_PAGE_VAL 100 // Maximum value of a page number for time tracking

// Helper function to print the current state of the frames
void print_frames(const int frame_array[], int frames, int page_fault) {
    for (int j = 0; j < frames; j++) {
        // Print -1 as '_' for a cleaner output, but still show the value
        if (frame_array[j] == -1) {
            printf("_ ");
        } else {
            printf("%d ", frame_array[j]);
        }
    }
    printf("| %s\n", (page_fault ? "YES" : "NO"));
}

// -------------------------------------------------------------------
// 1. FIFO Simulation
// -------------------------------------------------------------------
void simulate_fifo(const int ref_string[], int pages, int frames) {
    // Variable Length Array (VLA) for the physical frames
    int frame_array[frames];
    for (int i = 0; i < frames; i++) {
        frame_array[i] = -1; // -1 indicates an empty frame
    }

    int page_faults = 0;
    int index = 0; // Tracks the oldest frame for replacement (FIFO pointer)

    printf("\n\n--- FIFO Algorithm Simulation (Frames: %d) ---\n", frames);
    printf("Page Reference | Frames | Page Fault\n");
    printf("--------------------------------------------\n");

    for (int i = 0; i < pages; i++) {
        int page = ref_string[i];
        int page_found = 0;
        
        printf("       %d         | ", page);

        // Check if the page is already in a frame (Page Hit)
        for (int j = 0; j < frames; j++) {
            if (frame_array[j] == page) {
                page_found = 1;
                break;
            }
        }

        if (page_found == 0) {
            // Page Fault: Replace the page at the current FIFO index
            frame_array[index] = page;
            index = (index + 1) % frames; // Circular replacement
            page_faults++;
            print_frames(frame_array, frames, 1);
        } else {
            // Page Hit
            print_frames(frame_array, frames, 0);
        }
    }

    printf("--------------------------------------------\n");
    printf("Total Page Faults (FIFO): %d\n", page_faults);
    printf("Total Page Hits (FIFO):   %d\n", pages - page_faults);
    printf("Hit Ratio (FIFO):         %.2f%%\n", (float)(pages - page_faults) * 100 / pages);
    printf("Miss Ratio (FIFO):        %.2f%%\n", (float)page_faults * 100 / pages);
}

// -------------------------------------------------------------------
// 2. LRU Simulation
// -------------------------------------------------------------------
void simulate_lru(const int ref_string[], int pages, int frames) {
    // VLA for the physical frames
    int frame_array[frames];
    // To store the last access time (index in the reference string) for each frame
    int last_used_time[frames]; 
    
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
            int lru_index = 0;
            int min_time = INT_MAX;

            // Find the least recently used page to replace
            for (int j = 0; j < frames; j++) {
                if (frame_array[j] == -1) { // Priority 1: Find an empty frame
                    lru_index = j;
                    min_time = -1; // Flag that an empty slot was found
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
            page_faults++;

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
// 3. Main Program (Handles user input)
// -------------------------------------------------------------------
int main() {
    int frames, pages;
    
    // --- Get Frames ---
    printf("Enter number of frames (max %d): ", MAX_FRAMES);
    if (scanf("%d", &frames) != 1 || frames <= 0 || frames > MAX_FRAMES) {
        printf("Invalid frame count. Exiting.\n");
        return 1;
    }

    // --- Get Pages Length ---
    printf("Enter number of page references (max %d): ", MAX_REF_LEN);
    if (scanf("%d", &pages) != 1 || pages <= 0 || pages > MAX_REF_LEN) {
        printf("Invalid page reference count. Exiting.\n");
        return 1;
    }

    // --- Get Reference String ---
    int page_reference[pages]; // Using VLA, which is supported by most C compilers
    printf("Enter the %d page references (separated by spaces or newlines): ", pages);
    for (int i = 0; i < pages; i++) {
        if (scanf("%d", &page_reference[i]) != 1) {
            printf("Invalid input for page reference. Only integers allowed. Exiting.\n");
            return 1;
        }
    }

    // --- Run Simulations ---
    simulate_fifo(page_reference, pages, frames);
    simulate_lru(page_reference, pages, frames);

    return 0;
}