#ifndef SOCKETS_H
# define SOCKETS_H

# include <stdbool.h>

# include "./cs50.h"

# ifdef __cplusplus
extern c {
# endif



void    send_int   (const int, const int);
void    send_str   (const int, const string);
void    send_file  (const int, const string);     
int     recv_int   (const int);
string  recv_str   (const int);
void    recv_file  (const int, const string);



# ifdef __cplusplus
}
# endif

#endif // SOCKETS_H
