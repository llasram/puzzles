#! /usr/bin/python

from __future__ import with_statement

import sys
import os
import cPickle as pickle
from itertools import izip
from support import *

ROWSEP = ('-' * ((3 * 4) + 3))

def print_solution(positions, solution):
    solution = matrixize(positions, solution)
    cube = ordered_cubify(solution)
    planes = []
    for plane in cube:
        rows = []
        for row in plane:
            rows.append('|'.join("%3d" % (x+1,) for x in row))
            rows.append(ROWSEP)
        planes.append(rows)
    for rows in izip(*planes):
        print '    '.join(rows)

def main(argv=sys.argv):
    progname = os.path.basename(argv[0])
    args = argv[1:]
    if len(args) != 1 and len(args) != (len(PATTERN) + 1):
        sys.stderr.write("usage: %s DIRECTIONS | FILE\n")
        return 1
    with open('positions.pkl', 'rb') as f:
        positions = pickle.load(f)
    if len(args) == 1:
        with open(args[0], 'r') as f:
            for i, line in enumerate(f):
                print ("Solution #%d" % (i+1,)).center(80)
                solution = [int(x) for x in line.split()]
                print_solution(positions, solution)
                print
    else:
        solution = [int(x) for x in args]
        print_solution(positions, solution)
    return 0

if __name__ == '__main__':
    sys.exit(main())
