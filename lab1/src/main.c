#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define panic(...) ({ printf(__VA_ARGS__); exit(1); })
#define let __auto_type

int main(const int argc, const char* const argv[]) {
    if (argc != 3) {
        panic("Expecting 2 file names as positional arguments");
    }
    let v = argv;
    puts(*v);
}
