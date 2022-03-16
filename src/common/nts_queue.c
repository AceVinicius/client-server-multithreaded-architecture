#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../lib/include/cs50.h"
#include "../../lib/include/nts_queue.h"



struct nts_queue_t {
    struct nts_queue_node_t *head;
    struct nts_queue_node_t *tail;
};

struct nts_queue_node_t {
    struct nts_queue_node_t *next;
    string data;
};



typedef struct nts_queue_t NTS_QUEUE;
typedef struct nts_queue_node_t NTS_QUEUE_NODE;



inline static bool    _is_empty  (NTS_QUEUE *);
inline static bool    _enqueue   (NTS_QUEUE *, const string );
inline static string  _dequeue   (NTS_QUEUE *);



/******************************************************************************
 ***                           INTERNAL FUNCTIONS                           ***
 ******************************************************************************/



inline static bool
_is_empty(NTS_QUEUE *queue) {
    return queue->head == NULL ? true : false;
}



inline static bool
_enqueue(NTS_QUEUE *queue, const string data) {
    NTS_QUEUE_NODE *new_node = (NTS_QUEUE_NODE *) calloc(1, sizeof(NTS_QUEUE_NODE));

    if (new_node == NULL) {
        perror("nts_queue: _enqueue: calloc");
        exit(EXIT_FAILURE);
    }

    new_node->data = calloc(strlen(data)+1, sizeof(char));

    if (new_node->data == NULL) {
        perror("nts_queue: _enqueue: calloc");
        free(new_node);
        return false;
    }

    memcpy(new_node->data, data, strlen(data)+1);

    if (queue->tail == NULL) {
        queue->head = new_node;
    } else {
        queue->tail->next = new_node;
    }

    queue->tail = new_node;

    return true;
}



inline static string 
_dequeue(NTS_QUEUE *queue) {
    NTS_QUEUE_NODE *temp = queue->head;
    string data = temp->data;

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



NTS_QUEUE *
nts_queue_create(void) {
    NTS_QUEUE *new_queue = (NTS_QUEUE *) calloc(1, sizeof(NTS_QUEUE));
    
    if (new_queue == NULL) {
        perror("nts_queue: nts_queue_create: calloc");
        return NULL;
    }

    return new_queue;
}



bool
nts_queue_is_empty(NTS_QUEUE *queue) {
    if (queue == NULL) {
        fputs("nts_queue: nts_queue_is_empty: NULL pointer given\n", stderr);
        exit(EXIT_FAILURE);
    }

    return  _is_empty(queue);
}



bool
nts_enqueue(NTS_QUEUE *queue, const string data)
{
    if (queue == NULL) {
        fputs("nts_queue: nts_enqueue: NULL pointer given\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (data == NULL) {
        return false;
    }

    return _enqueue(queue, data);
}



string 
nts_dequeue(NTS_QUEUE *queue)
{
    if (queue == NULL) {
        fputs("nts_queue: nts_dequeue: NULL pointer given\n", stderr);
        exit(EXIT_FAILURE);
    }
    
    return _is_empty(queue) ? NULL : _dequeue(queue);
}



inline void
nts_queue_destroy(NTS_QUEUE *queue) {
    if (queue != NULL) {
        for (NTS_QUEUE_NODE *curr = queue->head; curr != NULL; curr = curr->next) {
            NTS_QUEUE_NODE *prev = curr;
            curr = curr->next;
            free(prev->data);
            free(prev);
        }

        free(queue);
    }
}
