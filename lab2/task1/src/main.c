#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "rust.h"
#include "process.h"

int main() {
    let pid = fork();

    printf("Returned pid is %i\n", pid);

    if (pid == 0) {
        printf("Code in child process (returned pid is 0):\n");

        let process = Process_current();
        Process_fmt(&process, stderr);

        printf("Finishing the child process\n");
    } else {
        printf("Code in parent process:\n");

        let process = Process_current();
        Process_fmt(&process, stderr);

        printf("Waiting for child to finish...\n");
        wait(NULL);

        printf("Parent process ended\n");
    }

    return 0;
}
