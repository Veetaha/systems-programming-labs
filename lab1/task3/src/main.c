#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <poll.h>

#include "rust.h"

int main(int argc, char *const argv[]) {
    if (argc != 2) {
        panic("Please speficy the label");
    }
    let label = argv[1];

    loop {
        struct pollfd fds[] = {{
            .fd = STDIN_FILENO,
            .events = POLLIN,
            .revents = 0
        }};

        let retval = poll(fds, 1, 5000);

        if (retval == -1) {
            panic("%s, error at select, %s", label, strerror(errno));
        } else if (retval) {
            char buff[1024] = {0};
            let len = read(STDIN_FILENO, buff, 1024) - 1;
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
