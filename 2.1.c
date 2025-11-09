#include<stdio.h>
#include<unistd.h>
#include <sys/wait.h>

void bubbleSort(int a[], int n){
    for (int i = 0; i<n; i++){
        for (int j = 0; j<n-1-i; j++){
            if(a[j]>a[j+1]){
                int temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
        }
    }
}

void insertSort(int a[], int n){
    for (int i = 0; i<n; i++){
        int key = a[i];
        int j = i-1;
        while(j>=0 && a[j]>key){
            a[j+1] = a[j];
            j--;
        }
        a[j+1] = key;
    }
}

int main(){
    int n;

    printf("Enter no of elements: ");
    scanf("%d", &n);
    int a[n], b[n];

    for (int i = 0; i<n; i++){
        printf("Enter element: ");
        scanf("%d", &a[i]);
        b[i] = a[i];
    }

    pid_t pid = fork();

    if(pid<0){
        printf("Fork failed");
        return 0;
    }

    if(pid==0){
        //Child Process
        insertSort(b, n);
        printf("Child Process\n");
        for(int i = 0; i<n; i++){
            printf("%d ", b[i]);
        }
        printf("\n");
        return 0;
    }else{
        //Parent Process
        sleep(10);
        bubbleSort(a, n);
        printf("Parent Process\n");
        for(int i = 0; i<n; i++){
            printf("%d ", a[i]);
        }
        printf("\n");
        wait(NULL);
        return 0;
    }

    return 0;
}
