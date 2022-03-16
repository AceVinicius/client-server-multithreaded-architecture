#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../lib/include/ts_queue.h"



struct ts_queue_t {
    struct ts_queue_node_t *head;
    struct ts_queue_node_t *tail;
    size_t data_size;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct ts_queue_node_t {
    void *data;
    struct ts_queue_node_t *next;
};



typedef struct ts_queue_t TS_QUEUE;
typedef struct ts_queue_node_t QUEUE_NODE;



inline static bool    _is_empty  (TS_QUEUE *);
inline static bool    _enqueue   (TS_QUEUE *, const void *);
inline static void *  _dequeue   (TS_QUEUE *);



/******************************************************************************
 ***                           INTERNAL FUNCTIONS                           ***
 ******************************************************************************/



inline static bool
_is_empty(TS_QUEUE *queue) {
    return queue->head == NULL ? true : false;
}



inline static bool
_enqueue(TS_QUEUE *queue, const void *data) {
    QUEUE_NODE *new_node = (QUEUE_NODE *) calloc(1, sizeof(QUEUE_NODE));

    if (new_node == NULL) {
        perror("ts_queue: _enqueue: calloc");
        return false;
    }

    new_node->data = calloc(1, queue->data_size);

    if (new_node->data == NULL) {
        perror("ts_queue: _enqueue: calloc");
        free(new_node);
        return false;
    }

    memcpy(new_node->data, data, queue->data_size);

    if (queue->tail == NULL) {
        queue->head = new_node;
    } else {
        queue->tail->next = new_node;
    }

    queue->tail = new_node;

    return true;
}



inline static void *
_dequeue(TS_QUEUE *queue) {
    if (_is_empty(queue)) {
        return NULL;
    }

    QUEUE_NODE *temp = queue->head;
    char *data = temp->data;
    
    queue->head = queue->head->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    free(temp);

    return data;
}



/******************************************************************************
 ***                           EXTERNAL FUNCTIONS                           ***
 ******************************************************************************/



TS_QUEUE *
ts_queue_create(const size_t data_size) {
    TS_QUEUE *new_queue = (TS_QUEUE *) calloc(1, sizeof(TS_QUEUE));
    
    if (new_queue == NULL) {
        perror("ts_queue: ts_queue_create: calloc");
        return NULL;
    }

    new_queue->data_size = data_size;

    pthread_cond_init(&new_queue->cond, NULL);
    pthread_mutex_init(&new_queue->mutex, NULL);

    return new_queue;
}



bool
ts_queue_is_empty(TS_QUEUE *queue) {
    if (queue == NULL) {
        fputs("ts_queue: ts_queue_is_empty: NULL pointer given", stderr);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&queue->mutex);
    
    const bool empty = _is_empty(queue);

    pthread_mutex_unlock(&queue->mutex);
    
    return empty;
}



bool
ts_enqueue(TS_QUEUE *queue, const void *data) {
    if (queue == NULL) {
        fputs("ts_queue: ts_enqueue: NULL pointer given", stderr);
        exit(EXIT_FAILURE);
    }

    if (data == NULL) {
        return false;
    }
    
    pthread_mutex_lock(&queue->mutex);

    bool status = _enqueue(queue, data);
    
    if (status) {
        pthread_cond_signal(&queue->cond);
    }
    pthread_mutex_unlock(&queue->mutex);

    return status;
}



void *
ts_dequeue(TS_QUEUE *queue) {
    if (queue == NULL) {
        fputs("ts_queue: ts_dequeue: NULL pointer given", stderr);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&queue->mutex);
    if (_is_empty(queue)) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    
    void *data = _dequeue(queue);

    pthread_mutex_unlock(&queue->mutex);

    return data;
}



inline void
ts_queue_destroy(TS_QUEUE *queue) {
    if (queue != NULL) {
        pthread_mutex_lock(&queue->mutex);

        for (QUEUE_NODE *curr = queue->head; curr != NULL; curr = curr->next) {
            QUEUE_NODE *prev = curr;
            curr = curr->next;
            free(prev->data);
            free(prev);
        }

        pthread_cond_destroy(&queue->cond);
        pthread_mutex_unlock(&queue->mutex);
        pthread_mutex_destroy(&queue->mutex);

        free(queue);
    }
}


inline void
ts_queue_wake_sleeping_workers(TS_QUEUE *queue, const size_t qtd_of_threads) {
    if (queue == NULL) {
        fputs("ts_queue: ts_queue_wake_sleeping_workers: NULL pointer given", stderr);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < qtd_of_threads; ++i) {
        pthread_cond_signal(&queue->cond);
    }
}
