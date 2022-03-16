#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include "../../lib/include/cs50.h"
#include "../../lib/include/sockets.h"
#include "../../lib/include/ts_hash_table.h"



struct hash_table_t {
    struct hash_table_item_t **array;
    size_t size;
    size_t data_size;
    pthread_mutex_t mutex;
    void (*free_data)(void *);
};



struct hash_table_item_t {
    string key;
    void *data;
    struct hash_table_item_t *next;
};



typedef struct hash_table_t HASH_TABLE;
typedef struct hash_table_item_t HASH_ITEM;



static        bool         _table_insert  (HASH_TABLE *, const string, const void *);
static        void *       _table_peek    (HASH_TABLE *, const string);
static        void         _table_delete  (HASH_TABLE *, const string);
static        size_t       _hash          (const size_t, const string);
static        HASH_ITEM *  _create_item   (const size_t, const string, const void *);
static inline void         _free_item     (HASH_TABLE *, HASH_ITEM *);


/******************************************************************************
 ***                           INTERNAL FUNCTIONS                           ***
 ******************************************************************************/



static size_t
_hash(const size_t table_size, const string key) {
    const size_t key_size = strlen(key);
    size_t hash = 17;

    for(size_t i = 0; i < key_size; ++i) {
        hash = hash * 13 + key[ i ];
    }

    return hash % table_size;
}



static HASH_ITEM *
_create_item(const size_t data_size, const string key, const void *data) {
    HASH_ITEM *new_item = (HASH_ITEM *) calloc(1, sizeof(HASH_ITEM));
    if (new_item == NULL) {
        perror("ts_hash_table: _create_item: calloc");
        return NULL;
    }

    const size_t key_size = strlen(key);

    new_item->key = (string) calloc(key_size+1, sizeof(char));
    if (new_item->key == NULL) {
        perror("ts_hash_table: _create_item: calloc");
        free(new_item);
        return NULL;
    }

    new_item->data = (void *) calloc(1, data_size);
    if (new_item->data == NULL) {
        perror("ts_hash_table: _create_item: calloc");
        free(new_item->key);
        free(new_item);
        return NULL;
    }

    new_item->next = NULL;

    memcpy(new_item->key , key , key_size);
    memcpy(new_item->data, data, data_size);

    new_item->key[key_size] = '\0';

    return new_item;
}



static inline void
_free_item(HASH_TABLE *table, HASH_ITEM *item) {
    table->free_data(item->data);
    free(item->data);
    free(item->key);
    free(item);
}



static bool
_table_insert(HASH_TABLE *table, const string key, const void *data) {
    const size_t index = _hash(table->size, key);

    HASH_ITEM *item = _create_item(table->data_size, key, data);

    if (item == NULL) {
        return false;
    }

    item->next = table->array[ index ];
    table->array[ index ] = item;

    return true;
}



static void *
_table_peek(HASH_TABLE *table, const string key) {
    const size_t index = _hash(table->size, key);

    HASH_ITEM *temp = table->array[ index ];

    while (temp != NULL && strcmp(temp->key, key) != 0) {
        temp = temp->next;
    }

    return temp != NULL ? temp->data : NULL;
}



static void
_table_delete(HASH_TABLE *table, const string key) {
    const size_t index = _hash(table->size, key);

    HASH_ITEM *temp = table->array[ index ];
    HASH_ITEM *prev = NULL;

    while (temp != NULL && strcmp(temp->key, key) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp != NULL) {
        if (prev == NULL) {
            table->array[ index ] = temp->next;
        } else {
            prev->next = temp->next;
        }

        _free_item(table, temp);
    }
}




/******************************************************************************
 ***                           EXTERNAL FUNCTIONS                           ***
 ******************************************************************************/



HASH_TABLE *
hash_table_create(const size_t table_size, const size_t data_size, void (*function)(void *)) {
    HASH_TABLE *new_table = (HASH_TABLE *) calloc(1, sizeof(HASH_TABLE));

    if (new_table == NULL) {
        perror("ts_hash_table: hash_table_create: calloc");
        return NULL;
    }

    new_table->array = (HASH_ITEM **) calloc(table_size, sizeof(HASH_ITEM *));

    if (new_table == NULL) {
        perror("ts_hash_table: hash_table_create: calloc");
        free(new_table);
        return NULL;
    }

    new_table->size      = table_size;
    new_table->data_size = data_size;
    new_table->free_data = function;
    pthread_mutex_init(&new_table->mutex, NULL);

    return new_table;
}



bool
hash_table_insert(HASH_TABLE *table, const string key, const void *data) {
    if (table == NULL || key == NULL || data == NULL) {
        fputs("ts_hash_table: hash_table_insert: NULL pointer given\n", stderr);
        return false;
    }

    pthread_mutex_lock(&table->mutex);

    bool status = true;
    if (_table_peek(table, key) != NULL) {
        // _table_delete
    }
    status = _table_insert(table, key, data);

    pthread_mutex_unlock(&table->mutex);

    return status;
}



void *
hash_table_peek(HASH_TABLE *table, const string key) {
    if (table == NULL || key == NULL) {
        fputs("ts_hash_table: hash_table_peek: NULL pointer given\n", stderr);
        return NULL;
    }

    pthread_mutex_lock(&table->mutex);

    void *data = _table_peek(table, key);

    pthread_mutex_unlock(&table->mutex);

    return data;
}



void
hash_table_delete(HASH_TABLE *table, const string key) {
    if (table != NULL && key != NULL) {
        pthread_mutex_lock(&table->mutex);
        
        _table_delete(table, key);
        
        pthread_mutex_unlock(&table->mutex);
    }
}



bool
hash_table_destroy(HASH_TABLE *table) {
    if (table == NULL) {
        return false;
    }

    pthread_mutex_lock(&table->mutex);

    for(size_t i = 0; i < table->size; ++i) {
        HASH_ITEM *curr = table->array[ i ];
        while(curr != NULL) {
            HASH_ITEM *prev = curr;
            curr = curr->next;
            _free_item(table, prev);
        }
    }

    pthread_mutex_unlock(&table->mutex);
    pthread_mutex_destroy(&table->mutex);
    free(table->array);
    free(table);

    return true;
}



void
hash_table_print(HASH_TABLE *table) {
    puts("Start");

    pthread_mutex_lock(&table->mutex);

    for (size_t index = 0; index < table->size; ++index) {
        printf("\t%zu ", index);

        if (table->array[ index ] == NULL) {
            printf("---");
        } else {
            for (HASH_ITEM *temp = table->array[ index ]; temp != NULL; temp = temp->next) {
                printf(" -> %s", temp->key);
            }
        }

        puts("");
    }

    pthread_mutex_unlock(&table->mutex);

    puts("End");
}



void
hash_table_list(HASH_TABLE *table, const int client_fd) {
    pthread_mutex_lock(&table->mutex);

    for (size_t index = 0; index < table->size; ++index) {
        if (table->array[ index ] != NULL) {
            for (HASH_ITEM *temp = table->array[ index ]; temp != NULL; temp = temp->next) {
                send_int(client_fd, 1);
                send_str(client_fd, temp->key);
            }
        }
    }

    pthread_mutex_unlock(&table->mutex);
    
    send_int(client_fd, -1);
}
