CC = gcc
CPPDEFINES = -D_GNU_SOURCE
CPPINCLUDES = -I.
CPPFLAGS += $(CPPINCLUDES) $(CPPDEFINES)
CFLAGS += -Wall -Wextra -Werror -std=gnu99 -O2 -ggdb3 -pipe
LDFLAGS =
LDLIBS = -lJudy
TEST = 
CODE = solve
DATA = positions.dat positions.pkl solutions.txt transforms.h
SOLUTION = snake-solution.txt

all: $(SOLUTION)

snake-solution.txt: printsol.py solutions.txt
	./printsol.py solutions.txt >$@

solutions.txt: solve positions.dat
	./solve >solutions.txt

positions.dat positions.pkl: gendata.py
	./gendata.py

transforms.h: transforms.py
	./transforms.py >$@

solve.o: solve.c cube.h transforms.h

solve: solve.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o *.pyc $(DATA) $(CODE) $(SOLUTION) $(TEST)

.PHONEY: clean
