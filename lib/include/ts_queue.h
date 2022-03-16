#ifndef TS_QUEUE_H
# define TS_QUEUE_H

# include <stddef.h>
# include <stdbool.h>

# ifdef __cplusplus
extern c {
# endif



typedef struct ts_queue_t TS_QUEUE;



TS_QUEUE *  ts_queue_create                 (const size_t);
bool        ts_queue_is_empty               (TS_QUEUE *);
bool        ts_enqueue                      (TS_QUEUE *, const void *);
void *      ts_dequeue                      (TS_QUEUE *);
void        ts_queue_destroy                (TS_QUEUE *);
void        ts_queue_wake_sleeping_workers  (TS_QUEUE *, const size_t);



# ifdef __cplusplus
}
# endif

#endif //   TS_QUEUE_H
