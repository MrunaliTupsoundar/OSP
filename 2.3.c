#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

/**
 * @brief Checks if a given integer is a prime number.
 *
 * @param n The integer to check.
 * @return true if the number is prime, false otherwise.
 */
bool isPrime(int n) {
    if (n <= 1) return false;
    // We only need to check up to the square root of n
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

/**
 * @brief Main function to parse arguments, fork a process, and generate primes.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return 0 on successful execution, 1 on error.
 */
int main(int argc, char *argv[]) { 
    long num_primes;

    // 1. Argument and Error Checking (Command Line Input)
    if (argc != 2) {
        return 1;
    }

    char *endptr;
    // Attempt to convert the string argument (the user's N) to a long integer
    num_primes = strtol(argv[1], &endptr, 10);

    // Check for conversion errors (e.g., non-numeric input, or number is negative/zero)
    if (*endptr != '\0' || endptr == argv[1] || num_primes <= 0) {
        fprintf(stderr, "Error: Input must be a positive integer greater than 0.\n");
        return 1;
    }

    // 2. Fork the process
    pid_t pid = fork();

    // 3. Handle Fork Error
    if (pid < 0) {
        perror("fork error");
        return 1;
    }

    // 4. Child Process Logic (pid == 0)
    if (pid == 0) {
        // This is the child process

        long count = 0;
        int current_num = 2;

        // Loop until 'num_primes' (the user's input N) primes have been found
        while (count < num_primes) {
            if (isPrime(current_num)) {
                printf("%d ", current_num);
                count++;
            }
            current_num++;
        }

        printf("\n"); // Add a newline after the sequence for clean output
        exit(0);
    }

    // 5. Parent Process Logic (pid > 0)
    else {
        // Parent uses the simplest wait call: wait(NULL).
        // It waits for the child to finish and ignores the exit status.
        wait(NULL); 
    }

    return 0;
}