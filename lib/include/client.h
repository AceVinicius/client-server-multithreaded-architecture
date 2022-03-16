#ifndef CLIENT_H
# define CLIENT_H

# include "./cs50.h"

# define WHITESPACE   " \t\r\n\v"
# define PROMPT_LIMIT         128
# define ATTEMPTS              15

# ifdef __cplusplus
extern c {
# endif



struct data_t {
    string token;
};

typedef struct data_t DATA;



# ifdef __cplusplus
}
# endif

#endif // CLIENT_H
