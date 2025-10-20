#!/bin/bash

if [ "$1" == "-e" ]; then
    echo "Launching Editor..."
    ./build/bin/Debug-Linux/Editor
else
    echo "Launching Game..."
    ./build/bin/Debug-Linux/OrigoGame
fi
