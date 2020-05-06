#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef struct Buffer Buffer;

Buffer* Buffer_new(uint32_t size);

char* Buffer_bytes(Buffer* self);
uint32_t Buffer_size(const Buffer* self);
ssize_t Buffer_read(Buffer* self, int fd);
ssize_t Buffer_write(Buffer* self, int fd, size_t total);

void Buffer_drop(Buffer* self);
