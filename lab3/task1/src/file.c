#include "file.h"
#include "rust.h"
#include <stdlib.h>

Option(FilePtr) File_open(const char* restrict path, const char* restrict mode) {
    let inner = fopen(path, mode);
    if (!inner) {
        return None(FilePtr);
    }
    let me = (File*)malloc(sizeof(File));
    *me = (File) { .inner = inner };
    return Some(FilePtr, me);
}

void File_write(File* self, const char* data) {
    fputs(data, self->inner);
}
void File_flush(File* self) {
    fflush(self->inner);
}

void File_drop(File* self){
    fclose(self->inner);
}
