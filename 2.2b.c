#include<stdio.h>
#include<stdlib.h>

#define MAX_SIZE 100

int binarySearch(int arr[], int l, int r, int key){
    while(l<=r){
        int mid = (l+r)/2;
        if(arr[mid]==key){
            return mid;
        }
        if(arr[mid]<key){
            l = mid+1;
        }else{
            r = mid-1;
        }
    }
    return -1;
}

int main(int argc, char* argv[]){
    if(argc<3){
        return 1;
    }

    int n = argc -2;

    int arr[MAX_SIZE];

    int search_key = argc-1;
    int key = atoi(argv[search_key]);

    for(int i = 0; i<n; i++){
        arr[i] = atoi(argv[i+1]);
    }

    int result_index = binarySearch(arr,0,n-1,key);

    if (result_index != -1) {
        printf("Child Process: Element %d FOUND at index %d.\n", key, result_index);
    } else {
        printf("Child Process: Element %d NOT found in the array.\n", key);
    }

    return 0;
}