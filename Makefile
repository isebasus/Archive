EXECA = encode
EXECB = decode

SOURCES_A = encode.c io.c pq.c node.c huffman.c code.c stack.c
OBJECTS_A = $(SOURCES_A:%.c=%.o)

SOURCES_B = decode.c io.c node.c huffman.c code.c stack.c pq.c
OBJECTS_B = $(SOURCES_B:%.c=%.o)

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

.PHONY: all clean format

all: $(EXECA) $(EXECB) $(EXECC) $(EXECD) 

$(EXECA): $(OBJECTS_A)
	$(CC) -o $@ $^ $(LFLAGS)

$(EXECB): $(OBJECTS_B)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -rf $(EXECA) $(EXECB) $(OBJECTS_A) $(OBJECTS_B)

scan-build: clean
	scan-build make 
