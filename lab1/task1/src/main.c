#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "rust.h"
#include "buffer.h"

define_option(int);
define_option(mode_t);


static Option(int) open_file(const char* name, int flags, Option(mode_t) mode) {
    let fd = mode.is_some
        ? open(name, flags, mode)
        : open(name, flags);

    if (fd < 0) {
        return None(int);
    }
    return Some(int, fd);
}

/**
 * Returns modified characters number
 */
static uint32_t to_uppercase(char* const str, const uint32_t total) {
    uint32_t total_uppercased = 0;
    for (uint32_t i = 0; i < total; ++i) {
        let uppercased = toupper(str[i]);
        total_uppercased += uppercased != str[i];
        str[i] = uppercased;
    }
    return total_uppercased;
}

typedef uint32_t (*MapperFn)(char* buf, uint32_t total);

/**
 * Returns total mapped bytes.
 */
static uint32_t pipe_mapped(int infile_fd, int outfile_fd, MapperFn map) {
    let buffer = Buffer_new(513);
    ssize_t bytes_read = 0;
    ssize_t total_bytes = 0;
    uint32_t total_mapped = 0;
    do {
        bytes_read = Buffer_read(buffer, infile_fd);
        total_bytes += bytes_read;
        total_mapped += map(Buffer_bytes(buffer), bytes_read);
        Buffer_write(buffer, outfile_fd, bytes_read);
    } while (bytes_read == Buffer_size(buffer));

    Buffer_drop(buffer);
    return total_mapped;
}


int main(const int argc, const char* const argv[]) {
    if (argc != 3) {
        panic("Expecting 2 file names as positional arguments");
    }

    let read_file_name = argv[1];
    let write_file_name = argv[2];

    println("Reading file %s, and writing to %s", read_file_name, write_file_name);

    let read_file = expect(
        open_file(read_file_name, O_RDONLY, None(mode_t)),
        "Failed to open file %s for read: %s", read_file_name, strerror(errno)
    );

    let write_file_opt = open_file(
        write_file_name, O_CREAT | O_WRONLY | O_APPEND, Some(mode_t, 0644)
    );
    if (!write_file_opt.is_some) {
        close(read_file);
        panic("Failed to open file %s for write: %s", write_file_name, strerror(errno));
    }
    let write_file = unwrap(write_file_opt);

    let mapped_bytes = pipe_mapped(read_file, write_file, to_uppercase);

    println("Changed %u bytes", mapped_bytes);

    if (fsync(write_file) < 0) {
        println("Failed at fsync(): %s", strerror(errno));
    }
    close(write_file);
    close(read_file);
}
