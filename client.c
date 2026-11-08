#include <sys/socket.h>
#include <netinet/in.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int s = socket(AF_INET6, SOCK_STREAM, 0);
    if (s == -1)
        err(EXIT_FAILURE, "socket");

    struct sockaddr_in6 addr = {
        .sin6_family = AF_INET6,
        .sin6_port = htons(8080),
        .sin6_addr = IN6ADDR_LOOPBACK_INIT
    };
    
    char *message = malloc(100);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        err(EXIT_FAILURE, "connect");

    printf("To close connection with the server write CLOSE_CONN\n(or press CTRL+C)\n");
    
    while (1) {
        if (fgets(message, 100, stdin) == NULL)
            break;                           

        message[strcspn(message, "\n")] = '\0'; 
        
        if (send(s, message, strlen(message), 0) == -1)
            err(EXIT_FAILURE, "send");
        if (strcmp(message, "CLOSE_CONN") == 0) {
            printf("Connection closed\n");
            break;
        }
    }
    
    close(s);

    free(message);
}