#!/bin/bash
gcc src/server.c -lpthread -lrt -o bin/server
gcc src/main.c -lncurses -lm -o bin/main
gcc src/watchdog.c -lpthread -lrt -o bin/watchdog


./bin/server 

