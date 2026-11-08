#include <sys/socket.h>
#include <err.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int s = socket(AF_INET6, SOCK_STREAM, 0);
    if (s == -1)
        err(EXIT_FAILURE, "socket");
    
    char *message = malloc(100);
    char *buffer = malloc(1024);

    struct sockaddr_in6 addr = {
        .sin6_family = AF_INET6,
        .sin6_port = htons(8080),
        .sin6_addr = IN6ADDR_LOOPBACK_INIT
    };

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        err(EXIT_FAILURE, "bind");

    if (listen(s, 5) == -1 ) 
        err(EXIT_FAILURE, "listen");

    printf("Your server works correctly and now can accept messages!\nTo close the server press CTRL+C\n");

    int client = accept(s, NULL, NULL);

    if (client == -1) 
        err(EXIT_FAILURE, "accept");

    if (buffer == NULL)
        err(EXIT_FAILURE, "malloc");

    while (1) {
        ssize_t n = recv(client, buffer, 1023, 0);
        if (n == -1)
            err(EXIT_FAILURE, "recv");

        if (n > 0) {
            buffer[n] = '\0';

            if (strcmp(buffer, "CLOSE_CONN") == 0) {
                printf("Connection with client was closed!\n");
                break;
            } else {
                printf("Accepted Message : %s\n", buffer);
            }
        }
    }

    close(s);
    free(message);
    free(buffer);
    
    return 0;
}