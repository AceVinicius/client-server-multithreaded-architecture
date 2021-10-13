#!/bin/bash

clang ./src/server/*.c ./src/common/*.c -lpthread -O3 -g3 -Wall -Wextra -Wpedantic -o ./bin/server

if [ "$?" -eq 0 ]; then
    valgrind --track-origins=yes --leak-check=full -s ./bin/server
fi
