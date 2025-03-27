PROJECT_NAME = simulator

CC = clang
CFLAGS = -Wall -Wextra -std=c23

MODE?=debug
DEBUGFLAGS = -g -O0 -Ddebug
RELEASEFLAGS = -O3 -lto -Drelease

ifeq ($(MODE),debug)
	CFLAGS += $(DEBUGFLAGS)
else
	CFLAGS += $(RELEASEFLAGS)
endif

TARGET = main

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
BIN = $(BIN_DIR)/$(PROJECT_NAME)

all: $(BIN)

$(BIN): $(OBJ)
	$(shell mkdir -p $(BIN_DIR))
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(shell mkdir -p $(OBJ_DIR))
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(BIN)
	@echo "Running $(BIN)"
	@echo ""
	@./$(BIN)