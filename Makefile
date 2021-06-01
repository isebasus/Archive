SOURCE = ./src
EXECA = encode
EXECB = decode

SOURCEA = src/code.o src/huffman.o src/io.o src/node.o src/pq.o src/stack.o src/encode.o
SOURCEB = src/code.o src/huffman.o src/io.o src/node.o src/pq.o src/stack.o src/decode.o

OBJECTS = $(patsubst $(SOURCE)/%.c,$(SOURCE)/%.o,$(wildcard $(SOURCE)/*.c))

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

.PHONY: all clean format

all: $(EXECA) $(EXECB)

$(EXECA): $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCEA) -o $(EXECA)

$(EXECB): $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCEB) -o $(EXECB)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -rf $(EXECA) $(EXECB) $(OBJECTS) 

scan-build: clean
	scan-build make 
