#ifndef NTS_QUEUE_H
# define NTS_QUEUE_H

# include <stdbool.h>

# include "./cs50.h"

# ifdef __cplusplus
extern c {
# endif



typedef struct nts_queue_t NTS_QUEUE;



NTS_QUEUE *  nts_queue_create    (void);
bool         nts_queue_is_empty  (NTS_QUEUE *);
bool         nts_enqueue         (NTS_QUEUE *, const string);
string       nts_dequeue         (NTS_QUEUE *);
void         nts_queue_destroy   (NTS_QUEUE *);



# ifdef __cplusplus
}
# endif

#endif //   NTS_QUEUE_H
