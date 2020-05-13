#include "process.h"

void Process_fmt(Process* self, FILE* out) {
    fprintf(out, "Process {\n\tpid: %i,\n", self->pid);
    fprintf(out, "\tppid: %i,\n", self->ppid);
    fprintf(out, "\tgid: %i,\n", self->gid);
    fprintf(out, "\tegid: %i,\n", self->egid);
    fprintf(out, "\tsid: %i,\n", self->sid);
    fprintf(out, "\tuid: %i,\n", self->uid);
    fprintf(out, "\teuid: %i\n}\n", self->euid);
}

Process Process_current() {
    return (Process) {
        .pid = getpid(),
        .ppid = getppid(),
        .gid = getgid(),
        .egid = getegid(),
        .sid = getsid(0),
        .uid = getuid(),
        .euid = geteuid()
    };
}
