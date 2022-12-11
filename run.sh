#!/bin/sh

if [ $# -ne 0 ]; then
    echo "Usage: No Arguments"
    exit
fi

echo "Preparing..."
make
echo ""
echo "..."
echo ""
echo "Hostname: "
hostname
echo "Executing Quartz"
./bin/main -r 2500 -lw1 0.7 -lw2 0.2 -lw3 0.1
