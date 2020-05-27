#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rust.h"

const size_t BUF_SIZE = 512;

define_option(ssize_t);

Option(ssize_t) write_buffer(int fd, const char *buffer, size_t size) {
    size_t i = 0;
    while (size > 0) {
        ssize_t wrote = write(fd, buffer, size);
        if (wrote < 0) return None(ssize_t);
        buffer += wrote;
        size -= wrote;
        i += wrote;
    }
    return Some(ssize_t, i);
}

void main_loop(int sock) {
    while (true) {
        char buffer[BUF_SIZE];

        if (!fgets(buffer, BUF_SIZE, stdin)) break;

        expect(write_buffer(sock, buffer, strlen(buffer)), "failed to write()");

        buffer[strcspn(buffer, "\n")] = 0;
        let should_stop = !strcmp(buffer, "close");

        let read_bytes = read(sock, buffer, BUF_SIZE - 1);
        if (read_bytes < 0) panic("Failed at read()");

        if (read_bytes == 0) {
            puts("No more data to read...");
            break;
        }

        buffer[read_bytes] = 0;
        println("Read:");
        fputs(buffer, stdout);

        if (should_stop) {
            break;
        }
    }
}

int main() {
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(3206);
    addr.sin_family = AF_INET;

    let sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) panic("Failed at socket()");

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))) {
        close(sock);
        panic("Failed at connect()");
    }

    main_loop(sock);
    close(sock);
}
