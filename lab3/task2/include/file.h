#pragma once

#include "rust.h"

typedef struct {
    FILE* inner;
} File;
typedef File* FilePtr;
define_option(FilePtr);

Option(FilePtr) File_open(const char* path, const char* mode);
void File_write(File* self, const char* data);
void File_flush(File* self);
void File_drop(File* self);
