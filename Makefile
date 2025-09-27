CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = src/server.c src/tmux_handler.c
OBJ = $(SRC:.c=.o)
OUT = server

all: $(OUT)

# Link object files into the final binary
$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Compile each .c file into a .o file
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT) $(OBJ)
