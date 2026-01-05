C++C = g++
CFLAGS = -std=c++23 -pedantic -Wall -Werror

main_run: main.o
	$(C++C) $(CFLAGS) main.o -o main
	rm main.o
	./main
	rm main

main: main.o
	$(C++C) $(CFLAGS) main.o -o main
	rm main.o

main.o: main.cpp
	$(C++C) $(CFLAGS) -c main.cpp -o main.o

clean:
	rm -f main.o
	rm -f main

.PHONY: clean