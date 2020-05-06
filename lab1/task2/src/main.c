#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/select.h>

#include "rust.h"

const size_t BUFFER_SIZE = 1024;

int main(int argc, char *const argv[]) {
    if (argc != 2) {
        panic("Please speficy the label");
    }
    let label = argv[1];

    loop {
        let timeout = (struct timeval){
            .tv_sec = 5,
            .tv_usec = 0
        };

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        let retval = select(1, &rfds, NULL, NULL, &timeout);
        if (retval == -1) {
            panic("%s, error at select, %s", label, strerror(errno));
        }
        if (retval) {
            char buff[BUFFER_SIZE];

            let len = read(STDIN_FILENO, buff, BUFFER_SIZE) - 1;
            if (buff[len] == '\n') {
                buff[len] = '\0';
            }
            println("%s: '%s'", label, buff);
        } else {
           println("%s: waiting for data...", label);
        }
    }

    return 0;
}
