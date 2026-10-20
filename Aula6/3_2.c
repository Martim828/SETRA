#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void delay(void) {
    for (int i = 0; i < 1000000000000000; i++) {
        // Busy-wait loop to simulate delay
    }
}

int main() {
    
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child process
        printf("Child process: PID = %d\n", getpid());
        delay(); // Simulate some work in the child process
        printf("Child process: Parent PID = %d\n", getppid());
        return EXIT_SUCCESS;
    }
    
    // Parent process
    printf("Parent process: PID = %d\n", getpid());
    printf("Parent process: Child PID = %d\n", pid);

    //wait(NULL);
    return 0;
}