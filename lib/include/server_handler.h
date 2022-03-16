#ifndef SERVER_HANDLER_H
# define SERVER_HANDLER_H

# include "./server.h"

# ifdef __cplusplus
extern c {
# endif



void  handle_client  (CLIENT_CON_INFO *peer_con);



# ifdef __cplusplus
}
# endif

#endif // SERVER_HANDLER_H
