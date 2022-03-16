#!/bin/zsh

if [ "$?" -eq 0 ]; then
    # valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s
    ./bin/server
fi
