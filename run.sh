#!/bin/sh

if [ $# -ne 0 ]; then
    echo "Usage: No Arguments"
    exit
fi

echo "Preparing..."
make > /dev/null 2>&1
echo "Executing Quartz"
./bin/main -r 5.0 -lw1 0.7 -lw2 0.2 -lw3 0.1