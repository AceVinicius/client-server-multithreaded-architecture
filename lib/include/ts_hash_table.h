#ifndef HASH_TABLE_H
# define HASH_TABLE_H

# include <stdbool.h>

#include "./cs50.h"

# define MAX_KEY 128

# ifdef __cplusplus
extern c {
# endif



typedef struct hash_table_t HASH_TABLE;



extern HASH_TABLE *  hash_table_create   (const size_t, const size_t, void (*)(void *));
extern bool          hash_table_destroy  (HASH_TABLE *);
extern bool          hash_table_insert   (HASH_TABLE *, const string, const void *);
extern void *        hash_table_peek     (HASH_TABLE *, const string);
extern void          hash_table_delete   (HASH_TABLE *, const string);
extern void          hash_table_print    (HASH_TABLE *);
extern void          hash_table_list     (HASH_TABLE *, const int);



# ifdef __cplusplus
}
# endif

#endif // HASH_TABLE_H
