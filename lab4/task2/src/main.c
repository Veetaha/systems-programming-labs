#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "rust.h"


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


int log_fd;

Option(ssize_t) info(const char* msg) {
    time_t t;
    time(&t);

    char b1[64];
    char b2[512];
    strftime(b1, 64, "%c", localtime(&t));

    return write_buffer(log_fd, b2, snprintf(b2, 512, "[%s] [info]: %s\n", b1, msg));
}

Option(ssize_t) error(const char* msg) {
    time_t t;
    time(&t);

    char b1[64];
    char b2[512];
    strftime(b1, 64, "%c", localtime(&t));

    return write_buffer(log_fd, b2, snprintf(b2, 512, "[%s] [error] %s: %s\n", b1, msg, strerror(errno)));
}

void main_loop(int client_socket, struct sockaddr_in* client_addr) {
    time_t t;
    char ip_buffer[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, (&client_addr->sin_addr), ip_buffer, INET_ADDRSTRLEN);

    while (true) {
        char in_buff[256];
        char out_buff[512];
        char time_buffer[64];
        ssize_t read_bytes;
        int msg_len;

        read_bytes = read(client_socket, in_buff, 255);
        if (read_bytes < 0) {
            snprintf(out_buff, 512, "Error during reading data from %s", ip_buffer);
            error(out_buff);
            break;
        }
        if (read_bytes == 0) { //client exit
            snprintf(out_buff, 512, "%s disconnected\n", ip_buffer);
            info(out_buff);
            break;
        }

        in_buff[read_bytes] = '\0';

        let do_close = false;

        if (!strcmp(in_buff, "close\r\n") || !strcmp(in_buff, "close\n")) {
            snprintf(out_buff, 512, "Closing session with %s\n", ip_buffer);
            info(out_buff);
            do_close = true;
        }

        time(&t);
        strftime(time_buffer, 64, "%c", localtime(&t));
        msg_len = snprintf(out_buff, 512, "(%jd) [%s]: %s", (intmax_t)getpid(), time_buffer, in_buff);
        if (!write_buffer(client_socket, out_buff, msg_len).is_some) {
            snprintf(out_buff, 512, "Error happaned while sending data to %s", ip_buffer);
            error(out_buff);
            break;
        }

        if (do_close) {
            break;
        }
    }

    if (close(client_socket) < 0) {
        error("Can't close client socket");
    }
}

void run_server(int port) {
    let sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) panic("Failed at socket()");

    info("Success: socket()");


    struct sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        panic("Failed at bind()");
    }
    info("Success: bind()");

    if (listen(sock, 5) < 0) panic("Failed at listen()");

    info("Success: listen()");

    while (true) {
        char ip_buffer[INET_ADDRSTRLEN];
        char buffer[256];

        struct sockaddr_in client_addr;
        socklen_t size = sizeof(client_addr);
        let client = accept(sock, (struct sockaddr*)&client_addr, &size); //waiting for
        if (client < 0) {
            error("Failed at accept()");
            continue;
        }

        inet_ntop(AF_INET, (&client_addr.sin_addr), ip_buffer, INET_ADDRSTRLEN);

        snprintf(buffer, 256, "Connection from ip: %s\n", ip_buffer);

        info(buffer);

        if (!fork()) {
            main_loop(client, &client_addr);
            exit(EXIT_SUCCESS);
        }
        close(client);
    }
}

int main() {

    int parent_fds[] = { STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO };

    let pid = fork();
    if (pid < 0) panic("Failed at fork()");
    if (pid > 0) return 0;

    if (setsid() < 0) panic("Failed at setsid()");

    pid = fork();
    if (pid < 0) panic("Failed at second fork()");
    if (pid > 0) return 0;

    if (chdir("/") < 0) panic("Failed at chdir()");

    for (int i = 0; i < 3; i++) close(parent_fds[i]);


    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w+", stdout);
    freopen("/dev/null", "w+", stderr);


    log_fd = open("/tmp/server.log", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (log_fd < 0) panic("failed at open()");

    run_server(3206);
    close(log_fd);
}
