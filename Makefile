CC = gcc
CFLAGS = -pedantic -Wall -Werror

main_run: main.o parser.o exec.o help.o formation.o error.o
	$(CC) $(CFLAGS) main.o parser.o exec.o help.o formation.o error.o -o main
	rm main.o
	rm parser.o
	rm exec.o
	rm help.o
	rm formation.o
	rm error.o
	./main
	rm main

main: main.o parser.o exec.o help.o formation.o error.o
	$(CC) $(CFLAGS) main.o parser.o exec.o help.o formation.o error.o -o main
	rm main.o
	rm parser.o
	rm exec.o
	rm help.o
	rm formation.o
	rm error.o

exec.o: src/exec/exec.c formation.o
	$(CC) $(CFLAGS) -c src/exec/exec.c -o exec.o

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o main.o

parser.o: src/parser/parser.c
	$(CC) $(CFLAGS) -c src/parser/parser.c -o parser.o

help.o: src/help/help.c
	$(CC) $(CFLAGS) -c src/help/help.c -o help.o

formation.o: src/formation/formation.c
	$(CC) $(CFLAGS) -c src/formation/formation.c -o formation.o

error.o: src/error/error.c
	$(CC) $(CFLAGS) -c src/error/error.c -o error.o

clean:
	rm -f main.o
	rm -f parser.o
	rm -f exec.o
	rm -f help.o
	rm -f formation.o
	rm -f error.o
	rm -f main

.PHONY: clean