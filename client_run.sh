#!/bin/bash

clang ./src/client/*.c ./src/common/*.c -O3 -g3 -Wall -Wextra -Wpedantic -o ./bin/client

$PORT
if [ -z "$1" ]; then
    PORT=2000
else
    PORT=$1
fi

if [ "$?" -eq 0 ]; then
    valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./bin/client "$PORT"
fi