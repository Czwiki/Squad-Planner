CC = gcc
CFLAGS = -pedantic -Wall -Werror

main_run: main.o parser.o
	$(CC) $(CFLAGS) main.o parser.o -o main
	rm main.o
	rm parser.o
	./main
	rm main

main: main.o parser.o
	$(CC) $(CFLAGS) main.o parser.o -o main
	rm main.o
	rm parser.o

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o main.o

parser.o: src/parser/parser.c
	$(CC) $(CFLAGS) -c src/parser/parser.c -o parser.o
clean:
	rm -f main.o
	rm -f main

.PHONY: clean