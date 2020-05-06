#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "rust.h"
#include "buffer.h"

typedef struct Buffer {
    char* bytes;
    uint32_t size;
} Buffer;


Buffer* Buffer_new(uint32_t size) {
    let newbie = (Buffer*)malloc(sizeof(Buffer));
    newbie->bytes = (char*)malloc(size * sizeof(char));
    newbie->size = size;
    return newbie;
}

ssize_t Buffer_read(Buffer* self, int fd) {
    return read(fd, self->bytes, self->size);
}

ssize_t Buffer_write(Buffer* self, int fd, size_t total) {
    println("%s", self->bytes);
    return write(fd, self->bytes, total);
}

char* Buffer_bytes(Buffer* self) {
    return self->bytes;
}

uint32_t Buffer_size(const Buffer* self) {
    return self->size;
}

void Buffer_drop(Buffer* self) {
    free(self->bytes);
    free(self);
}
