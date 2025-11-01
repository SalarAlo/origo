#!/bin/bash

if [ "$1" == "-e" ]; then
    echo "Launching Editor..."
    ./build/bin/Debug-Linux/OrigoEditor
else
    echo "Launching Game..."
    ./build/bin/Debug-Linux/OrigoGame
fi
