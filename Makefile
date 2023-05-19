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

main : $(OBJS)/main.o $(OBJS)/list.o $(OBJS)/page.o $(OBJS)/util.o $(OBJS)/multiprocess.o
	$(CC) $^ -o $@

.PHONY: all clean

clean:
	rm -f $(OBJS)/*.o
