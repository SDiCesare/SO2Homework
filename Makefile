# Compiler Option

CC := gcc
CFLAGS := -O2

# Directories
SRC := src
OBJS := bin

#=======================

PROGS := main


all: $(PROGS)

$(OBJS)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

main : $(OBJS)/*.o
	$(CC) $^ -o $@

.PHONY: all clean

clean:
	rm -f $(OBJS)/*.o