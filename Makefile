CC = gcc
CFLAGS = -pedantic -Wall -Werror

main_run: main.o parser.o exec.o help_commands.o formation.o
	$(CC) $(CFLAGS) main.o parser.o exec.o help_commands.o formation.o -o main
	rm main.o
	rm parser.o
	rm exec.o
	rm help_commands.o
	rm formation.o
	./main
	rm main

main: main.o parser.o exec.o help_commands.o formation.o
	$(CC) $(CFLAGS) main.o parser.o exec.o help_commands.o formation.o -o main
	rm main.o
	rm parser.o
	rm exec.o
	rm help_commands.o
	rm formation.o

exec.o: src/exec/exec.c formation.o
	$(CC) $(CFLAGS) -c src/exec/exec.c -o exec.o

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c -o main.o

parser.o: src/parser/parser.c
	$(CC) $(CFLAGS) -c src/parser/parser.c -o parser.o

help_commands.o: src/exec/help_commands.c
	$(CC) $(CFLAGS) -c src/exec/help_commands.c -o help_commands.o

formation.o: src/FormationContext/formation.c
	$(CC) $(CFLAGS) -c src/FormationContext/formation.c -o formation.o

clean:
	rm -f main.o
	rm -f parser.o
	rm -f exec.o
	rm -f help_commands.o
	rm -f formation.o
	rm -f main

.PHONY: clean