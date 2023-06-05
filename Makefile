# Compiler Option

CC := gcc
CFLAGS := -O2

# Directories
SRC_DIR := src
BIN_DIR := bin

#=======================

PROGS := main


all: $(PROGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

main : $(OBJS)
	$(info [${OBJS}])
	$(CC) $^ -o $@

.PHONY: all clean

clean:
	rm -f $(BIN_DIR)/*.o
