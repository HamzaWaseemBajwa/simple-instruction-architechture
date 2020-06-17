CC=g++
CFLAGS=-I. -Wall
DEPS = ALU.h Memory.h Registers.h
OBJ = ALU.o Memory.o Registers.o siavm.cpp

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

siavm: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

all: siavm

clean:
	rm siavm *.o
