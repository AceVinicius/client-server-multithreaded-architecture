#ifndef SERVER_H
# define SERVER_H

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



int PORT;



# ifdef __cplusplus
}
# endif

#endif // SERVER_H
