#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "rust.h"
#include "file.h"

File *log_file;

void signal_handler(int _signo, siginfo_t *si, void *_ctx) {
    if (_ctx || _signo) {;}

    int bruh_si_signo = si->si_signo;
    int bruh_si_code = si->si_code;
    int bruh_si_errno = si->si_errno;
    int bruh_si_pid = si->si_pid;
    int bruh_si_uid = si->si_uid;

    println_to(
        log_file->inner,
        "siginfo_t {\n"
        "\t si_signo: %d,\n"
        "\t si_code: %d,\n"
        "\t si_errno: %d,\n"
        "\t si_pid: %d,\n"
        "\t si_uid: %d,\n"
        "}",
        bruh_si_signo,
        bruh_si_code,
        bruh_si_errno,
        bruh_si_pid,
        bruh_si_uid
    );
    File_flush(log_file);
}

int main(int argc, char **argv) {
    if (argc < 2) {
      panic("Invalid arguments, path is not specified");
    }
    log_file = expect(File_open(argv[1], "w+"), "couldn't open the log file");

    println_to(log_file->inner, "pid: %d", getpid());

    struct sigaction sig_action = {
        .sa_sigaction = &signal_handler,
        .sa_flags = SA_SIGINFO
    };

    if (sigaction(SIGHUP, &sig_action, NULL) == -1) {
        println_to(log_file->inner, "Failed to register signal handler: %s", strerror(errno));
        File_flush(log_file);
        File_drop(log_file);
        exit(1);
    }
    while (true) {
        println_to(log_file->inner, "Waiting...\n");
        File_flush(log_file);
        sleep(3);
    }
}
