#!/usr/bin/env bash

if [ -z "$1" ]; then
    echo 'USAGE: ./install_dependencies [apt|dnf]'
else
    sudo "$1" install clang valgrind
fi
