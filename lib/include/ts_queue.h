#ifndef QUEUE_H
# define QUEUE_H

# include <stddef.h>
# include <stdbool.h>

# ifdef __cplusplus
extern c {
# endif



typedef struct queue_t QUEUE;



QUEUE *  queue_create                 (const size_t);
bool     queue_is_empty               (QUEUE *);
bool     enqueue                      (QUEUE *, const void *);
void *   dequeue                      (QUEUE *);
void     queue_destroy                (QUEUE *);
void     queue_wake_sleeping_workers  (QUEUE *, const size_t);



# ifdef __cplusplus
}
# endif

#endif //   QUEUE_H
