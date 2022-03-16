#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../lib/include/common.h"
#include "../../lib/include/client_handler.h"
#include "../../lib/include/cs50.h"
#include "../../lib/include/sockets.h"
#include "../../lib/include/nts_queue.h"



int
list(NTS_QUEUE *tokens, const int server_fd) {
    int receiving;

    do {
        receiving = recv_int(server_fd);

        if (receiving == 1) {
            char *tmp = recv_str(server_fd);
            puts(tmp);
            free(tmp);
        }
    } while (receiving != -1);

    return RUNNING_CLIENT;
}



int
download(NTS_QUEUE *tokens, const int server_fd) {
    if (tokens == NULL) {
        fputs("client: search: NULL pointer given\n", stderr);
        return RUNNING_CLIENT;
    }

    for (string token = nts_dequeue(tokens); token != NULL; token = nts_dequeue(tokens)) {
        send_int(server_fd, true);
        send_str(server_fd, token);

        if (recv_int(server_fd) == false) {
            fprintf(stderr, "search: %s cannot be stored in server\n", token);
        }

        free(token);
    }

    send_int(server_fd, false);

    return RUNNING_CLIENT;
}
