CC = gcc
CFLAGS = -pedantic -Wall -Werror

# Note for Windows builds (MSVC / MinGW):
# - Replace 'rm -f' with 'del /Q' or use a cross-platform build tool (CMake).
# - With MSVC, replace 'gcc' with 'cl' and adapt flags accordingly.
# - MinGW provides gcc and rm via MSYS2, so this Makefile works there as-is.
# - The src/compat.h header handles strdup/_strdup and unistd.h differences.

OBJS = main.o parser.o exec.o help.o formation.o error.o persistence.o cJSON.o squad.o player.o
# objects to link into fuzz harness (exclude main.o to avoid duplicate main)
# OBJS_CORE = parser.o exec.o help.o formation.o error.o persistence.o cJSON.o squad.o player.o

main_run: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o Squad_Planner -lm
	rm -f $(OBJS)
	./Squad_Planner
	rm -f Squad_Planner

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o Squad_Planner -lm
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

squad.o: src/squad/squad.c
	$(CC) $(CFLAGS) -c src/squad/squad.c -o squad.o

player.o: src/player/player.c
	$(CC) $(CFLAGS) -c src/player/player.c -o player.o

clean:
	rm -f $(OBJS)
	rm -f Squad_Planner

.PHONY: clean

# AFL_CC ?= afl-clang-fast


# .PHONY: fuzz setup_testcases


# fuzz: setup_testcases
# 	@echo "Building object files with $(AFL_CC) (excluding main.o)..."
# 	$(MAKE) CC=$(AFL_CC) CFLAGS="-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer" $(OBJS_CORE)
# 	@echo "Linking instrumented fuzz harness (no main.o)..."
# 	$(AFL_CC) -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer fuzz/fuzz_harness.c $(OBJS_CORE) -o fuzz/fuzz_harness -lm
# 	@echo "Built fuzz/fuzz_harness"

# setup_testcases:
# 	@mkdir -p fuzz/testcases
# 	@cp -r saves/* fuzz/testcases/ 2>/dev/null || true
# 	@printf "\n" > fuzz/testcases/empty
# 	@printf "{}" > fuzz/testcases/trunc.json
# 	@echo "fuzz/testcases prepared (copied from saves/)"