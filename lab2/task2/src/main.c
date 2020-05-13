#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include "process.h"
#include "rust.h"
#include "file.h"


struct tm *now() {
    time_t rawtime;
    time(&rawtime);
    return localtime(&rawtime);
}

int main() {
    {
        let logFile = expect(File_open("./logfile2.log", "w"), "Failed to create a log file");

        fprintf(logFile->inner, "Started execution at %s\n", asctime(now()));
        File_flush(logFile);

        pid_t childPid = fork();
        if (childPid < 0) {
            File_drop(logFile);
            panic("Failed to fork");
        }

        if (childPid > 0) {
            fprintf(logFile->inner, "Child successfully is created (its pid: %i)\n", childPid);

            File_drop(logFile);
            return 0;
        }
        File_drop(logFile);
    }

    {
        pid_t sid = setsid();
        if (sid < 0) panic("Couldn't set sid");

        println("Created new seesion with sid: %i", sid);
    }

    if (chdir("/") < 0) panic("Failed to change directory to the root");

    // int fdMax = getdtablesize();
    // for (int i = 0; i < fdMax; i++) close(i);

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    let process = Process_current();
    let logFile = expect(File_open("/home/veetaha/dev/systems-programming-labs/lab2/task2/logfile2.log", "a+t"), "BRUH!");
    Process_fmt(&process, stderr);
    Process_fmt(&process, logFile->inner);

    File_flush(logFile);

    let i = 0;
    while (1) {
        sleep(1);
        fprintf(logFile->inner, "Bro bruh (%i)...\n", i);
        File_flush(logFile);
        ++i;
    }
    File_drop(logFile);
}
