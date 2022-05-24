CC=clang
CFLAGS=-g -Wall

BIN=bin/task2
BARG=name.txt

all: $(BIN)

$(BIN): src/main.c
	$(CC) $(CFLAGS) -o $@ $^

run: $(BIN)
	./$(BIN) $(BARG)

gdb: $(BIN)
	gdb ./$(BIN)

mem: $(BIN)
	valgrind --leak-check=full --show-reachable=yes -s ./$(BIN) $(BARG)

clean:
	rm $(BIN)
