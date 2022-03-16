#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../lib/include/common.h"
#include "../../lib/include/cs50.h"
#include "../../lib/include/server.h"
#include "../../lib/include/sockets.h"
#include "../../lib/include/ts_queue.h"
#include "../../lib/include/ts_hash_table.h"



void
handle_client(CLIENT_CON_INFO *peer_con) {
    switch (recv_int(peer_con->fd)) {
        case LIST_ID:
            break;

        case DOWNLOAD_ID:
            break;

        default:
            break;
    }
}
