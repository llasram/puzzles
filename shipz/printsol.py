#! /usr/bin/python

from __future__ import with_statement

import sys
import os
import cPickle as pickle
from support import *

def squarify(coords):
    square = array([[-1]*MAXZ for i in xrange(MAXY)])
    maxz = -1
    minz = 5
    for x, y, z in coords:
        if z > square[x, y]:
            square[x, y] = z
        maxz = max(maxz, z)
        minz = min(minz, z)
    return (minz, maxz, square.tolist())

def outchar(val):
    if val == -1:
        return "*"
    return "%d" % (val,)

def print_solution(pieces, solution):
    solution = sorted(squarify(pieces[x]) for x in solution)
    solution = [square for _, _, square in solution]
    rows = [[] for i in xrange(6)]
    for i, square in enumerate(solution):
        rows[0].append(("Piece #%d:" % (i + 1,)).center(16))
        for j, row in enumerate(square):
            line = ''.join(outchar(c) for c in row)
            rows[j+1].append(line.center(16))
        if (i % 5) != 4:
            continue
        for row in rows:
            print ''.join(row)
        print
        rows = [[] for i in xrange(6)]

def main(argv=sys.argv):
    progname = os.path.basename(argv[0])
    args = argv[1:]
    if len(args) != 1 and len(args) != 25:
        sys.stderr.write("usage: %s PIECE*25 | FILE")
        return 1
    with open('pieces.pkl', 'rb') as f:
        pieces = pickle.load(f)
    if len(args) == 1:
        with open(args[0], 'r') as f:
            for i, line in enumerate(f):
                print ("Solution #%d" % (i+1,)).center(80)
                solution = [int(x, 16) for x in line.split()]
                print_solution(pieces, solution)
                print
    else:
        solution = [int(x, 16) for x in args]
        print_solution(pieces, solution)
    return 0

if __name__ == '__main__':
    sys.exit(main())
