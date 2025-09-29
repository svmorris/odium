CC = gcc
CFLAGS = -Wall -Wextra -O2
DEBUGFLAGS = -Wall -Wextra -g -O0

# Sources
SERVER_SRC = src/server.c src/tmux_handler.c
CLIENT_SRC = src/client.c

# Objects
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Outputs
SERVER_OUT = odium
CLIENT_OUT = odium-client-internal

# Default target: build everything
all: $(SERVER_OUT) $(CLIENT_OUT)

# Build server binary
server: $(SERVER_OUT)

$(SERVER_OUT): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJ)

# Build client binary
client: $(CLIENT_OUT)

$(CLIENT_OUT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJ)

# Debug build (both server and client with debug flags)
debug: clean
	$(MAKE) CFLAGS="$(DEBUGFLAGS)" all

# Generic rule to compile .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run helpers
run-server: $(SERVER_OUT)
	./$(SERVER_OUT)

run-client: $(CLIENT_OUT)
	./$(CLIENT_OUT)

# Cleanup
clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT) $(SERVER_OBJ) $(CLIENT_OBJ)
