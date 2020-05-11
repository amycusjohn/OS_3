CC = gcc
CFLAGS = -g

TARGET1 = master
TARGET2 = bin_adder
OBJS1 = master.o
OBJS2 = bin_adder.o

all: $(TARGET1) $(TARGET2)

master: $(OBJS1)
	$(CC) $(CFLAGS)  -g -Wall -lpthread -lrt -lm -o master master.o

bin_adder: $(OBJS2)
	$(CC) $(CFLAGS)  -g -Wall -lpthread -lrt -lm -o bin_adder bin_adder.o

%.o: %.c  $(HEADER)
	gcc -c $(CFLAGS) $*.c -o $*.o

clean:
	/bin/rm -f *.o  master bin_adder adder_log.txt
