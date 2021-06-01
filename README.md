> :warning: **This Project is a modified school project** and thus no students of Dr.Long should be looking at my source code.

# 📦 Archive
## A Huffman coding implementation for file compression and decompression.

This project builds two programs (the encoder and the decoder). Both programs use the Huffman Coding technique to compress or decompress bytes within a file. The user will need to pipe in desired files to encode or decode.

## What you need

In order to compile this program you'll need the Makefile provided, encode.c and decode.c. You'll also need all the .h files as dependencies. 

### Building Archive

To build the program, you will need [Clang](https://askubuntu.com/a/1058535) installed. Once installed, run the following commands below. 

Once inside the Archive directory, run:

```
$ make all
```
This will build all programs including encode.c and entropy.c

To run encode.c:

```
$ ./encode [-h] [-v] [-i infile] [-o outfile]
```

The arguments needed to run this program are:

```
  -h             Program usage and help.
  -v             Print compression statistics.
  -i infile      Input data to encode.
  -o outfile     Output of encoded data.
```

To run decode.c:

```
$ ./decode [-h] [-v] [-i infile] [-o outfile]
```

The arguments needed to run this program are:

```
  -h             Program usage and help.
  -v             Print out compression statistics.
  -i infile      Input data to encode.
  -o outfile     Output of encoded data.
```

You can run both files together like:

```
$ ./encode < path/to/file | ./decode 
```

To remove the binary files run:

```
$ make clean
```

### 📚 Sources
* [The C Programming Language - 2nd Edition](https://drive.google.com/file/d/0BxImv9GQ06DcNjc1MTk0N2ItZmJmNC00YTM0LWFmN2UtMGQ4ZmE3Y2E1YzU4/view) - IO Inspiration 
* [Introduction to Algorithms 3rd Edition](https://github.com/RbkGh/Free-Algorithm-Books/blob/master/book/Introduction%20to%20Algorithms%20-%20Third%20Edition.pdf) - ADT Implementation Inspiration
* Sample Files Provided by my Professor Dr. Long
* Stackoverflow 

### 📋 Licensing
This project is licensed under the MIT license. I hope others can learn from my code, but you know who your are if you shouldn't be looking at this code (aka Dr. Long Students). Other than that, all of this code is completely original and you can modify anything as much as you wish!

