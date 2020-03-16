CC = gcc

all: master bin_adder

.SUFFIXES: .c .o

master: main.c
	gcc -g -Wall -lpthread -lrt -lm -o master main.c

bin_adder: child.c
	gcc -g -Wall -lpthread -lrt -lm -o bin_adder child.c

clean:
	$(RM) master bin_adder *.txt *.o
