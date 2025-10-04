# Forgive me for using chatgpt here.
CC = gcc
# Strict compile for normal builds
CFLAGS = -Wall -Wextra -Werror -O2
# Debug compile (no Werror, includes debug symbols)
DEBUGFLAGS = -Wall -Wextra -g -O0

# Directories
BIN_DIR = bin
BUILD_DIR = build

# Sources
SERVER_SRC = src/server.c src/tmux_handler.c
CLIENT_SRC = src/client.c

# Objects (in build dir)
SERVER_OBJ = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(CLIENT_SRC))

# Outputs (in bin dir)
SERVER_OUT = $(BIN_DIR)/odium
CLIENT_OUT = $(BIN_DIR)/odium-client-internal

# Default target: build everything strictly
all: $(SERVER_OUT) $(CLIENT_OUT)

# Build odium (server)
odium: $(SERVER_OUT)

$(SERVER_OUT): $(SERVER_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJ)

# Build odium-client-internal
odium-client-internal: $(CLIENT_OUT)

$(CLIENT_OUT): $(CLIENT_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJ)

# Generic rule to compile .c -> .o into build dir
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Debug build (both odium and client with debug flags)
debug: clean
	$(MAKE) CFLAGS="$(DEBUGFLAGS)" all

# Ensure bin/ and build/ dirs exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run helpers
run-odium: $(SERVER_OUT)
	./$(SERVER_OUT)

run-client: $(CLIENT_OUT)
	./$(CLIENT_OUT)

# Cleanup
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
