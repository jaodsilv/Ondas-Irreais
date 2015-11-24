ondas-irreais: main.o
	gcc -o ondas-irreais main.o -g

main.o: main.c
	gcc -c main.c -o main.o -Wall -Wextra -pedantic -g

clean:
	rm -rf *.o
	rm ondas-irreais

all: ondas-irreais
