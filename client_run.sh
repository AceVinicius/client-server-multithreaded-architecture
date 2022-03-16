#!/bin/zsh

$PORT
if [ -z "$1" ]; then
    PORT=2000
else
    PORT=$1
fi

if [ "$?" -eq 0 ]; then
    # valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s 
    ./bin/client "$PORT"
fi
