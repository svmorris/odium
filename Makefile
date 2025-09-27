CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = src/demo.c
OUT = server

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
