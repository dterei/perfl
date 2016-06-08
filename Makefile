CC=cc
CFLAGS=-O3
BIN=test
SRC_FILES=test.c work.c

all: $(BIN)

$(BIN): $(SRC_FILES)

.PHONY: clean
clean:
	rm -f $(BIN) *.o
