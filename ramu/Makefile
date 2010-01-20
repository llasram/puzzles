CC = gcc
CPPDEFINES = -D_GNU_SOURCE
CPPINCLUDES = -I.
CPPFLAGS += $(CPPINCLUDES) $(CPPDEFINES)
CFLAGS += -Wall -Wextra -Werror -std=gnu99 -O2 -ggdb3 -pipe
LDFLAGS =
LDLIBS = 
TEST = run-tests
CODE = solve
DATA = pieces.dat pieces.pkl transforms.h
SOLUTION = ramu-solution.txt

all: $(SOLUTION)

test: run-tests
	./run-tests

ramu-solution.txt: solve pieces.dat printsol.py
	./printsol.py `./solve` >$@

pieces.dat pieces.pkl: gendata.py
	./gendata.py

transforms.h: transforms.py
	./transforms.py >$@

solve.o: solve.c cube.h transforms.h
run-tests.o: run-tests.c cube.h transforms.h

solve: solve.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

run-tests: run-tests.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.pyc *.o $(DATA) $(CODE) $(SOLUTION) $(TEST)

.PHONEY: clean
