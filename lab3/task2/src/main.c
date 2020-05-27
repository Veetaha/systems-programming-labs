#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "rust.h"

typedef struct {
    pid_t pid;
    time_t timestamp;
    char message[1024];
} Datum;

void Datum_print(const Datum* self) {
    printf(
        "Datum {\n"
        "\tpid: %d,\n"
        "\ttimestamp: %ld,\n"
        "\tmessage: %s,\n",
        self->pid,
        self->timestamp,
        self->message
    );
}

const size_t BUF_SIZE = 1024;

int main() {
    let fd = shm_open("my_memory", O_CREAT | O_RDWR, S_IRWXU);
    if (fd < 0) {
        panic("Failed at shm_open(): %s", strerror(errno));
    }
    if (ftruncate(fd, sizeof(Datum)) < 0) {
        panic("Failed at ftruncate(): %s", strerror(errno));
    }

    Datum* datum = mmap(NULL, sizeof(datum), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (datum == MAP_FAILED) {
        panic("failed at mmap(): %s", strerror(errno));
    }

    const pid_t pid = getpid();
    char str[BUF_SIZE];

    while (true) {
        println("reading the data");
        scanf("%s", str);
        msync(datum, sizeof(datum), MS_SYNC);
        Datum_print(datum);
        strcpy(datum->message, str);
        datum->pid = pid;
        datum->timestamp = time(0);
    }
}
