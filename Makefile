CC = gcc
CFLAGS = -pedantic -Wall -Werror

# Note for Windows builds (MSVC / MinGW):
# - Replace 'rm -f' with 'del /Q' or use a cross-platform build tool (CMake).
# - With MSVC, replace 'gcc' with 'cl' and adapt flags accordingly.
# - MinGW provides gcc and rm via MSYS2, so this Makefile works there as-is.
# - The src/compat.h header handles strdup/_strdup and unistd.h differences.

OBJS = main.o parser.o exec.o help.o formation.o error.o persistence.o cJSON.o

main_run: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main -lm
	rm -f $(OBJS)
	./main
	rm -f main

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main -lm
	rm -f $(OBJS)

exec.o: src/exec/exec.c formation.o persistence.o
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

persistence.o: src/persistence/persistence.c cJSON.o
	$(CC) $(CFLAGS) -c src/persistence/persistence.c -o persistence.o

cJSON.o: lib/cJSON/cJSON.c
	$(CC) -pedantic -Wall -c lib/cJSON/cJSON.c -o cJSON.o

clean:
	rm -f $(OBJS)
	rm -f main

.PHONY: clean