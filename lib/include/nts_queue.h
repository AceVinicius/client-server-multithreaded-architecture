#ifndef QUEUE_H
# define QUEUE_H

# include <stdbool.h>

# include "../../lib/include/cs50.h"

# ifdef __cplusplus
extern c {
# endif



typedef struct queue_t QUEUE;



QUEUE *  queue_create    (void);
bool     queue_is_empty  (QUEUE *);
bool     enqueue         (QUEUE *, const string);
string   dequeue         (QUEUE *);
void     queue_destroy   (QUEUE *);



# ifdef __cplusplus
}
# endif

#endif //   QUEUE_H
