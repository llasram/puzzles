#! /usr/bin/python

from __future__ import with_statement

import sys
import os
import cPickle as pickle
from itertools import izip
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

def main(argv=sys.argv):
    progname = os.path.basename(argv[0])
    args = argv[1:]
    if len(args) != 8:
        sys.stderr.write("usage: %s PIECE*8\n")
        return 1
    with open('pieces.pkl', 'rb') as f:
        pieces = pickle.load(f)
    options = [p[int(x, 16)] for x, p in izip(args, pieces)]
    solution = sorted(squarify(option) for option in options)
    solution = [square for _, _, square in solution]
    for i, square in enumerate(solution):
        print "Piece #%d:" % (i + 1,)
        print '\n'.join('  ' + ''.join(outchar(c) for c in r) for r in square)
        print
    negative = list(product(xrange(MAXX), repeat=3))
    negative = set(tuple(c) for c in negative)
    negative.difference_update(set([c for o in options for c in o]))
    negative.difference_update(set(VERBOTEN))
    _, _, negative = squarify(negative)
    print "Negative space:"
    print '\n'.join('  ' + ''.join(outchar(c) for c in r) for r in negative)
    print
    return 0

if __name__ == '__main__':
    sys.exit(main())
