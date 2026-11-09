#include <sys/socket.h>
#include <err.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

int main() {
    int s = socket(AF_INET6, SOCK_STREAM, 0);
    if (s == -1)
        err(EXIT_FAILURE, "socket");
    
    char *buffer = malloc(1024);

    struct sockaddr_in6 addr = {
        .sin6_family = AF_INET6,
        .sin6_port = htons(8080),
        .sin6_addr = IN6ADDR_LOOPBACK_INIT
    };

    struct pollfd fds[2];

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        err(EXIT_FAILURE, "bind");

    if (listen(s, 5) == -1 ) 
        err(EXIT_FAILURE, "listen");

    printf("Your server works correctly and now can accept messages!\nTo close the server press CTRL+C\n");

    fds[0].fd = accept(s, NULL, NULL);
    fds[0].events = POLLIN;

    fds[1].fd = accept(s, NULL, NULL);
    fds[1].events = POLLIN;

    if (fds[0].fd == -1 || fds[1].fd == -1) 
        err(EXIT_FAILURE, "accept");

    if (buffer == NULL)
        err(EXIT_FAILURE, "malloc");

    while (1) {

        if (poll(fds, 2, -1) == -1)
            err(EXIT_FAILURE, "poll");

        for (int i = 0; i < 2; i++) {

            if (!(fds[i].revents & POLLIN))
                continue;

            ssize_t n = recv(fds[i].fd, buffer, 1023, 0);

            if (n == -1)
                err(EXIT_FAILURE, "recv");

            if (n == 0) {
                close(fds[i].fd);
                fds[i].fd = -1;
                continue;
            }

            buffer[n] = '\0';

            if (strcmp(buffer, "CLOSE_CONN") == 0) {
                printf("Connection with User%d was closed!\n", i + 1);
                close(fds[i].fd);
                fds[i].fd = -1;
            } else {
                send(fds[1 - i].fd, buffer, n, MSG_NOSIGNAL);
                printf("<User%d>%s\n", i + 1, buffer);
            }
        }
        if (fds[0].fd == -1 && fds[1].fd == -1)
            break;
    }

    close(s);
    free(buffer);
    
    return 0;

}