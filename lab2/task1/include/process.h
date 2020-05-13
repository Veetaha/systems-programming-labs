#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    pid_t pid;
    pid_t ppid;
    gid_t gid;
    gid_t egid;
    pid_t sid;
    uid_t uid;
    uid_t euid;
} Process;

void Process_fmt(Process* self, FILE* out);
Process Process_current();
