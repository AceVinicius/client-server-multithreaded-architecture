#ifndef CLIENT_HANDLER_H
# define CLIENT_HANDLER_H

# include "./client.h"
# include "./nts_queue.h"

# ifdef __cplusplus
extern c {
# endif



int  list      (NTS_QUEUE *, const int);
int  download  (NTS_QUEUE *, const int); 



# ifdef __cplusplus
}
# endif

#endif // CLIENT_HANDLER_H
