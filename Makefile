CC = gcc

# Strict compile
CFLAGS = -Wall -Wextra -Werror -O2
DEBUGFLAGS = -Wall -Wextra -g -O0
DEPFLAGS = -MMD -MP

BIN_DIR = bin
BUILD_DIR = build

# All source files for the unified odium binary
SRC = src/main.c src/client.c src/tmux_handler.c

# Objects and dependency files
OBJ = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEPS = $(OBJ:.o=.d)

# Output binary
OUT = $(BIN_DIR)/odium

# Default target
all: $(OUT)

# Link odium
$(OUT): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Compile .c → .o (auto dependency generation)
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Debug build
debug: clean
	$(MAKE) CFLAGS="$(DEBUGFLAGS)" all

# Make directories
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run
run: $(OUT)
	./$(OUT)

# Cleanup
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

# Include auto-generated deps
-include $(DEPS)
