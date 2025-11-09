#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

#define MAX_SIZE 100
#define MAX_ARG_LEN 12

void bubbleSort(int arr[], int n){
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n-1-i; j++){
            if(arr[j]>arr[j+1]){
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

int main(){
    int n, key;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    int arr[MAX_SIZE];

    for(int i = 0; i<n; i++){
        scanf("%d", &arr[i]);
    }

    printf("Enter element to search");
    scanf("%d", &key);

    bubbleSort(arr, n);

    char args[MAX_SIZE+2][MAX_ARG_LEN];
    char *argv[MAX_SIZE+3];

    pid_t pid = fork();

    if(pid<0){
        printf("Fork Failed");
        return 1;
    }

    if (pid == 0){
        argv[0] = "./child";

        for(int i = 0; i<n; i++){
            snprintf(args[i+1], MAX_ARG_LEN, "%d", arr[i]);
            argv[i+1] = args[i+1];
        }

        snprintf(args[n+1], MAX_ARG_LEN, "%d", key);
        argv[n+1] = args[n+1];

        argv[n+2] = NULL;

        execv(argv[0], argv);

        perror("execv failed");
        _exit(1);
    }else{
        wait(NULL);
        printf("Parent Process: Child finished execution.\n");
    }

    return 0;
}