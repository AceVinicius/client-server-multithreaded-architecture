#ifndef SERVER_H
# define SERVER_H

# include "./cs50.h"

#define MAX_CONCURRENT_CONNECTIONS 2048
#define THREAD_POOL_SIZE             24

# ifdef __cplusplus
extern c {
# endif



struct client_connection_info_t {
    int fd;
    short port;
    string ip;
};



typedef struct client_connection_info_t CLIENT_CON_INFO;




# ifdef __cplusplus
}
# endif

#endif // SERVER_H
