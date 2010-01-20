CC = gcc
CPPDEFINES = -D_GNU_SOURCE
CPPINCLUDES = -I.
CPPFLAGS += $(CPPINCLUDES) $(CPPDEFINES)
CFLAGS += -Wall -Wextra -Werror -std=gnu99 -O2 -ggdb3 -pipe
LDFLAGS =
LDLIBS = -lJudy
TEST = run-tests
CODE = initials solve
DATA = pieces.dat pieces.pkl occupations.dat occupations.pkl \
	   initials.txt solutions.txt transforms.h
SOLUTION = shipz-solution.txt

all: $(SOLUTION)

test: run-tests
	./run-tests

pieces.dat occupations.dat: gendata.py
	./gendata.py

initials.txt: initials pieces.dat occupations.dat
	./initials >$@

solutions.txt: solve initials.txt pieces.dat occupations.dat
	./solve initials.txt >$@

shipz-solution.txt: printsol.py solutions.txt
	./printsol.py solutions.txt >$@

transforms.h: transforms.py
	./transforms.py >$@

initials.o: initials.c cube.h transforms.h
solve.o: solve.c cube.h transforms.h
run-tests.o: run-tests.c cube.h transforms.h

initials: initials.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

solve: solve.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

run-tests: run-tests.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o *.pyc $(DATA) $(CODE) $(SOLUTION) $(TEST)

.PHONEY: clean
