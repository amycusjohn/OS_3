all: master bin_adder
master: main.c
	gcc -o master main.c -lpthread
bin_adder: child.c
	gcc -o bin_adder child.c -lpthread
clean:
	-rm  master bin_adder

