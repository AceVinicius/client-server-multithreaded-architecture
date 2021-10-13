#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../lib/include/cs50.h"
#include "../../lib/include/sockets.h"
#include "../../lib/include/ts_queue.h"
#include "../../lib/include/ts_hash_table.h"



#define SERVER_IP                  "127.0.0.1"
#define SERVER_PORT                       8000
#define MAX_CONCURRENT_CONNECTIONS        2048

#define THREAD_POOL_SIZE 24

#define EXIT_CLIENT    false
#define RUNNING_CLIENT true

#define UNKNOWN_ID -1
#define EXIT_ID     0
#define LIST_ID     1
#define SEARCH_ID   2



struct url_info_t {
    string url;
    size_t page_content_size;
    string page_content;
};

struct client_connection_info_t {
    int fd;
    short port;
    string ip;
};

typedef struct client_connection_info_t CLIENT_CON_INFO;
typedef struct url_info_t URL_INFO;



HASH_TABLE *database;

pthread_mutex_t event_loop_status_mutex;
volatile bool event_loop_status = RUNNING_CLIENT;



void
free_data(void *data) {
    if (data != NULL) {
        URL_INFO *temp = (URL_INFO *) data;
        free(temp->url);
        free(temp->page_content);
    }
}



void
handle_client(CLIENT_CON_INFO *peer_con) {
    switch (recv_int(peer_con->fd)) {
        case LIST_ID:
            hash_table_list(database, peer_con->fd);
            break;

        case SEARCH_ID:
            while(recv_int(peer_con->fd) == true) {
                URL_INFO *url_info = (URL_INFO *) calloc(1, sizeof(URL_INFO));
                url_info->url = recv_str(peer_con->fd);
                hash_table_insert(database, url_info->url, url_info); 
                free(url_info);
            }

            break;

        default:
            break;
    }
}



void *
handle_client_queue(void *arg) {
    CLIENT_CON_INFO *peer_con;

    bool worker_status = RUNNING_CLIENT;

    while (worker_status) {
        peer_con = (CLIENT_CON_INFO *) dequeue((QUEUE *) arg);

        if (peer_con != NULL) {
            handle_client(peer_con);
            free(peer_con);
        } else {
            pthread_mutex_lock(&event_loop_status_mutex);
            event_loop_status = EXIT_CLIENT;
            pthread_mutex_unlock(&event_loop_status_mutex);

            worker_status = EXIT_CLIENT;
        }
    }

    free(peer_con);

    return NULL;
}



static int
fake_client(void) {
    struct sockaddr_in server_address;
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) { 
        perror("server: fake_client: socket");
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_family = AF_INET;

    if(inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) != 1) {
        perror("server: fake_client: inet_pton");
        return -1;
    }

    if (connect(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("server: fake_client: connect");
        return -1;
    }

    return server_fd;
}



static int
start_server(void) {
    struct sockaddr_in my_addr;
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) { 
        perror("server: sert_server: socket");
        return -1;
    }

    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("server: start_server: setsockopt");
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
        return -1;
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(SERVER_PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    

    if (bind(server_fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1) {
        perror("server: start_server: bind");
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
        return -1;
    }

    return server_fd;
}



void *
event_loop(void *arg) {
    const int server_fd = start_server();

    if (server_fd == -1) {
        pthread_mutex_lock(&event_loop_status_mutex);
        event_loop_status = EXIT_CLIENT;
        pthread_mutex_unlock(&event_loop_status_mutex);
    }
    
    while (event_loop_status) {
        if (listen(server_fd, MAX_CONCURRENT_CONNECTIONS) == -1) {
            perror("server: event_loopt: listen");

            pthread_mutex_lock(&event_loop_status_mutex);
            event_loop_status = EXIT_CLIENT;
            pthread_mutex_unlock(&event_loop_status_mutex);

            break;
        }

        CLIENT_CON_INFO *peer_con = (CLIENT_CON_INFO *) calloc(1, sizeof(CLIENT_CON_INFO));
        
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_size = sizeof(struct sockaddr_in);

        peer_con->fd = accept(server_fd, (struct sockaddr *) &peer_addr, &peer_addr_size);
        
        if (peer_con->fd == -1) {
            perror("server: event_loopt: accept");

            pthread_mutex_lock(&event_loop_status_mutex);
            event_loop_status = EXIT_CLIENT;
            pthread_mutex_unlock(&event_loop_status_mutex);

            break;
        }

        peer_con->ip   = inet_ntoa(peer_addr.sin_addr);
        peer_con->port = recv_int(peer_con->fd);

        bool is_in_queue = enqueue((QUEUE *) arg, peer_con);
        send_int(peer_con->fd, is_in_queue);

        if (is_in_queue) {
            fprintf(stdout, "\nlog: Enqueued client %s:%d\n>>> ", peer_con->ip, peer_con->port);
        } else {
            fprintf(stdout, "\nlog: Client not enqueued\n>>> ");
        }

        free(peer_con);
    }

    // Terminate server
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);

    return NULL;
}



int
main(void) {
    bool status = RUNNING_CLIENT;

    // Initialise data structures
    QUEUE *client_queue = queue_create(sizeof(CLIENT_CON_INFO));
    database = hash_table_create(2, sizeof(URL_INFO), free_data);

    // Initialise worker threads
    pthread_t workers[ THREAD_POOL_SIZE ];

    for (size_t i = 0; i < THREAD_POOL_SIZE; ++i) {
        if(pthread_create(&workers[ i ], NULL, handle_client_queue, client_queue) != 0) {
            perror("server: main: pthread_create");

            pthread_mutex_lock(&event_loop_status_mutex);
            event_loop_status = EXIT_CLIENT;
            pthread_mutex_unlock(&event_loop_status_mutex);

            status = EXIT_CLIENT;
        }
    }

    // Initialise main thread
    pthread_t server_thread;

    pthread_mutex_init(&event_loop_status_mutex, NULL);
    if(pthread_create(&server_thread, NULL, event_loop, client_queue) != 0) {
        perror("server: main: pthread_create");

        pthread_mutex_lock(&event_loop_status_mutex);
        event_loop_status = EXIT_CLIENT;
        pthread_mutex_unlock(&event_loop_status_mutex);

        status = EXIT_CLIENT;
    }

    while (status) {
        // Allocate and read user input
        string prompt = get_string(">>> ");

        // Cleaning queue leftovers
        if (strcmp(prompt, "exit") == 0) {
            pthread_mutex_lock(&event_loop_status_mutex);
            event_loop_status = EXIT_CLIENT;
            pthread_mutex_unlock(&event_loop_status_mutex);

            status = EXIT_CLIENT;
        }
    }

    // Terminate main thread
    const int fake_client_fd = fake_client();

    if (fake_client_fd != -1) {
        shutdown(fake_client_fd, SHUT_RDWR);
        close(fake_client_fd);
    } else {
        fputs("server: main: Couldn't fake client to terminate event_loop thread", stderr);
    }

    pthread_join(server_thread, NULL);
    pthread_mutex_destroy(&event_loop_status_mutex);

    // Terminate all worker threads
    queue_wake_sleeping_workers(client_queue, MAX_CONCURRENT_CONNECTIONS);
    for (size_t i = 0; i < THREAD_POOL_SIZE; ++i) {
        pthread_join(workers[ i ], NULL);
    }

    // Free all allocated memory
    queue_destroy(client_queue);
    hash_table_destroy(database);

    return EXIT_SUCCESS;
}
