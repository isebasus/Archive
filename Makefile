EXECA = encode
EXECB = decode
SRC = ./src/util
IO = ./src/io
HMAN = ./src/huffman

SOURCEA = $(SRC)/code.o $(HMAN)/huffman.o $(IO)/io.o $(SRC)/node.o $(SRC)/pq.o $(SRC)/stack.o src/encode.o
SOURCEB = $(SRC)/code.o $(HMAN)/huffman.o $(IO)/io.o $(SRC)/node.o $(SRC)/pq.o $(SRC)/stack.o src/decode.o

OBJECTSA = $(SOURCEA:%.c=%.o)
OBJECTSB = $(SOURCEB:%.c=%.o)

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

.PHONY: all clean

all: $(EXECA) $(EXECB)

$(EXECA): $(OBJECTSA)
	$(CC) $(CFLAGS) $(SOURCEA) -o $(EXECA)

$(EXECB): $(OBJECTSB)
	$(CC) $(CFLAGS) $(SOURCEB) -o $(EXECB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(EXECA) $(EXECB) $(OBJECTSA) $(OBJECTSB) 

scan-build: clean
	scan-build make 
