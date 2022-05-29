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
	valgrind --track-origins=yes --leak-check=full --show-reachable=yes -s ./$(BIN) $(BARG)

ARCHIVE_CMD=git archive --format $@ -o ./task2.$@ main

tgz:
	$(ARCHIVE_CMD)

zip:
	$(ARCHIVE_CMD)

clean:
	rm $(BIN)
