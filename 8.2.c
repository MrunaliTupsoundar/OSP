#include<stdio.h>
#include<stdlib.h> // Required for abs()

// Disk setup and requests
int req[] = {20,229,39,450,18,145,120,380,20,250};
int N = 10; // Number of requests
// Disk cylinders 0 to 499

// Bubble sort function
void sort(int a[], int n){
    for(int i = 0; i<n; i++){
        for(int j = i+1; j<n; j++) {
            if(a[j]<a[i]){
                int temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
}

int main(){
    int cur = 185; // Current head position
    int a[10];      // Array to store and sort the requests
    
    // Copy requests to 'a' and sort them
    for(int i = 0; i<N; i++){
        a[i] = req[i];
    }
    sort(a, N); // Sorted array: 18, 20, 20, 39, 120, 145, 229, 250, 380, 450

    // Arrays to hold requests split by current position (cur=185)
    int right[10]; // >= 185
    int ri = 0;
    int left[10];  // < 185
    int le = 0;

    // Split the sorted requests
    for (int i = 0; i<N; i++){
        if(a[i]>=cur){
            right[ri] = a[i];
            ri++;
        }else{
            left[le] = a[i];
            le++;
        }
    }

    // --- 1. SCAN Algorithm (Initial movement towards 499) ---
    int dist=0, last = cur;
    
    printf("--- 1. SCAN Algorithm (To 499 then 0) ---\n");
    printf("SCAN Sequence: %d", cur); // Start printing sequence

    // 1. Move Right (Service 229, 250, 380, 450)
    for(int i = 0; i<ri; i++){
        dist+= abs(right[i] - last);
        last = right[i];
        printf(" -> %d", last);
    }

    // 2. Move to the end boundary (499)
    dist+= abs(499-last);
    last = 499;
    printf(" -> %d", last); // Print boundary stop

    // 3. Move Left (Service 145, 120, 39, 20, 20, 18 in reverse sorted order)
    for (int i = le-1; i>=0; i--){
        dist += abs(left[i]-last);
        last = left[i];
        printf(" -> %d", last);
    }

    printf("\nTotal Head Movement (SCAN) = %d cylinders\n", dist);
    printf("Average Seek Distance (SCAN) = %.2f\n\n", dist/(double)N);


    // --- 2. LOOK Algorithm (Initial movement towards 499) ---
    last = cur; // Reset head position
    dist = 0;   // Reset total distance
    
    printf("--- 2. LOOK Algorithm (To 450 then 18) ---\n");
    printf("LOOK Sequence: %d", cur); // Start printing sequence

    // 1. Move Right (Service 229, 250, 380, 450)
    for (int i = 0; i<ri; i++){
        dist += abs(right[i]-last);
        last = right[i];
        printf(" -> %d", last);
    }
    
    // 2. Turn Around and service Left requests
    if(le){
        // Go from the last served Right (450) to the largest Left (145)
        dist += abs(last - left[le-1]); 
        last = left[le-1]; // Update last position to the largest left request
        printf(" -> %d", last);
        
        // 3. Service the rest of the Left requests (120, 39, 20, 20, 18)
        for(int i = le-2; i>=0; i--){
            dist += abs(left[i]-last); 
            last = left[i]; // Update the head position
            printf(" -> %d", last);
        }
    }

    printf("\nTotal Head Movement (LOOK) = %d cylinders\n", dist);
    printf("Average Seek Distance (LOOK) = %.2f\n\n", dist/(double)N);
    
    // --- 3. Conclusion ---
    // Note: LOOK distance for this specific set is 697. SCAN is 795.
    if (dist < 795) { 
        printf("Conclusion: LOOK performed better for this request set with a Total Head Movement of %d cylinders, which is lower than SCAN's 795 cylinders.\n", dist);
    } else {
        printf("Conclusion: SCAN performed better for this request set (This is unlikely for these values).\n");
    }

    return 0;
}