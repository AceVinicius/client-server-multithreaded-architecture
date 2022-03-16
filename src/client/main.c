#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../lib/include/client.h"
#include "../../lib/include/client_handler.h"
#include "../../lib/include/common.h"
#include "../../lib/include/cs50.h"
#include "../../lib/include/sockets.h"
#include "../../lib/include/nts_queue.h"



static int
connect_to_server(void) {
    struct sockaddr_in server_address;
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) { 
        perror("client: connect_to_server: socket");
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_family = AF_INET;

    if(inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) != 0) {
        perror("client: connect_to_server: inet_pton");
        return -1;
    }

    if (connect(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("client: connect_to_server: connect");
        return -1;
    }

    return server_fd;
}



static int
execute_cmd(NTS_QUEUE *tokens, const int cmd_id) {
    if (tokens == NULL) {
        fputs("client: execute_cmd: NULL pointer given\n", stderr);
        return RUNNING_STATE;
    }

    // Start server connection
    const int server_fd = connect_to_server();
    bool status = RUNNING_STATE;

    if (server_fd == -1) {
        return status;
    }

    send_int(server_fd, PORT);

    // Confirm connections was queued
    if (recv_int(server_fd) == false) {
        fputs("client: execute_cmd: Server is busy\n", stderr);
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
        return status;
    }

    send_int(server_fd, cmd_id);

    switch (cmd_id) {
        case LIST_ID:
            status = list(tokens, server_fd);
            break;

        case DOWNLOAD_ID:
            status = download(tokens, server_fd);
            break;

        default:
            break;
    }

    send_int(server_fd, -1);
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);

    return status;
}



static int
translate_command_into_id(const string command) {
    if (command == NULL) {
        return UNKNOWN_ID;
    }

    if (strcmp(command, EXIT_CMD) == 0) {
        return EXIT_ID;
    } else if (strcmp(command, LIST_CMD) == 0) {
        return LIST_ID;
    } else if (strcmp(command, SEARCH_CMD) == 0) {
        return SEARCH_ID;
    }

    return UNKNOWN_ID;
}



static int
pre_execute_cmd(NTS_QUEUE *tokens, const string command) {
    if (tokens == NULL) {
        fputs("client: pre_execute_cmd: NULL pointer given\n", stderr);
        return RUNNING_STATE;
    }

    const int cmd_id = translate_command_into_id(command);
    bool status = RUNNING_STATE;

    switch (cmd_id) {
        case EXIT_ID:
            status = EXIT_STATE;
            break;

        case LIST_ID:
        case SEARCH_ID:
            status = execute_cmd(tokens, cmd_id);
            break;

        default:
            fprintf(stdout, "client: %s: command not found...\n", command);
            break;
    }

    return status;
}



static string 
parse_prompt(NTS_QUEUE *tokens, string prompt) {
    if (tokens == NULL) {
        fputs("client: parse_prompt: NULL pointer given\n", stderr);
        return NULL;
    }

    string command = strtok(prompt, WHITESPACE);

    while (nts_enqueue(tokens, strtok(NULL, WHITESPACE)));

    return command;
}



int
main(const int argc, const char **argv) {
    if (argc != 2) {
        fputs("USAGE: ./client [PORT_NUMBER]\n", stderr);
        return EXIT_FAILURE;
    }

    if (sscanf(argv[ 1 ], "%i", &PORT) != 1) {
        fputs("client: error: PORT_NUMBER is not an integer\n", stderr);
        fputs("USAGE: ./client [PORT_NUMBER]\n", stderr);
        return EXIT_FAILURE;
    }

    NTS_QUEUE *tokens = nts_queue_create();
    if (tokens == NULL) {
        return EXIT_FAILURE;
    }

    bool status = RUNNING_STATE;

    do {
        // Allocate and read user input
        string prompt = get_string(">>> ");

        // Parse and execute desired command
        const string command = parse_prompt(tokens, prompt);
        if (command == NULL) {
            continue;
        }

        status = pre_execute_cmd(tokens, command);

        // Cleaning queue leftovers
        for (string token = nts_dequeue(tokens); token != NULL; token = nts_dequeue(tokens)) {
            free(token);
        }
    } while (status);

    nts_queue_destroy(tokens);

    return EXIT_SUCCESS;
}
