#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../lib/include/cs50.h"



void
send_int(const int socket, const int number) {
    if (send(socket, &number, sizeof(int), 0) == -1) {
        perror("sockets: send_int: send");
        exit(EXIT_FAILURE);
    }
}



int
recv_int(const int socket) {
    int number;
    const int bytes_recvd = recv(socket, &number, sizeof(int), 0);

    if (bytes_recvd == -1) {
        perror("sockets: recv_int: recv: error receiving int");
        exit(EXIT_FAILURE);
    }

    return number;
}



void
send_str(const int socket, const string text) {
    const long length = strlen(text);

    send_int(socket, length);

    const long bytes_sent = send(socket, text, sizeof(char) * length, 0);
    if (length != bytes_sent) {
        fputs("sockets: send_str: send: string sizes do not match\n", stderr);
        exit(EXIT_FAILURE);
    }
}



string 
recv_str(const int socket) {
    const long length = recv_int(socket);
    string text = (string) calloc(length+1, sizeof(char));

    const long bytes_recvd = recv(socket, text, sizeof(char) * length, 0);
    if (bytes_recvd == -1) {
        perror("sockets: recv_str: recv");
        return NULL;
    } else if (bytes_recvd != length) {
        fputs("sockets: recv_str: recv: string sizes do not match\n", stderr);
        return NULL;
    }

    return text;
}
