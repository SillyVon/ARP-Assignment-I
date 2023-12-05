CC = gcc
CFLAGS = -Wall
LIBS = -lpthread -lrt -lncurses -lm

BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include
LOGS_DIR = logs

all: $(BIN_DIR)/server $(BIN_DIR)/main $(BIN_DIR)/watchdog

$(BIN_DIR)/server: $(SRC_DIR)/server.c $(INCLUDE_DIR)/main.h
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

$(BIN_DIR)/main: $(SRC_DIR)/main.c $(INCLUDE_DIR)/main.h
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

$(BIN_DIR)/watchdog: $(SRC_DIR)/watchdog.c $(INCLUDE_DIR)/main.h
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

.PHONY: clean

clean:
	rm -f $(BIN_DIR)/server $(BIN_DIR)/main $(BIN_DIR)/watchdog
