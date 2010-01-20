#! /usr/bin/python

from __future__ import with_statement

import sys
import math
from math import sin, cos
import copy
import cPickle as pickle
import struct
import numpy
from numpy import matrix, array
from collections import defaultdict
from itertools import izip
from support import *

PROVIDED = [
    "0 222" "     " "     " "     " "C III"
    "0227 " "3    " "     " "     " "CII  "
    "00 7 " "3    " "     " "C    " "C    "
    "30 77" "3    " "     " "C    " "  OO "
    "3   7" "     " "     " " NNN " "OOONN",
    
    "0 222" "     " "     " " 999 " "99JJJ"
    "022  " "3    " "     " "A    " " JJ  "
    "00   " "3    " "     " "A    " "     "
    "30   " "3    " "    N" "A   N" "A    "
    "3   N" "   ON" "   ON" "   OO" "A   O",

    "0 222" "     " "     " " 999 " "A  99"
    "022  " "3    " "     " "A    " "AJ   "
    "00   " "3    " "     " "A    " "JJ   "
    "30   " "3    " "    N" "A   N" "J    "
    "3   N" "   ON" "   ON" "   OO" "J   O",

    "0  44" "     " "    G" "    G" "AA  G"
    "0444 " "5   G" "    G" "     " " AAA "
    "00   " "5    " "     " "     " "     "
    "50   " "5    " "     " "     " " MMM "
    "5  II" " III " "     " "  NN " "NNNMM",
    ]

def rotations(piece):
    rotset = set()
    for x, y, z in product(ANGLES, repeat=3):
        p = (piece * rot_matrix(x, y, z)).round(0)
        minp = matrix([p[:,0].min(), p[:,1].min(), p[:,2].min()])
        p -= minp
        p1 = [(int(c[0, 0]), int(c[0, 1]), int(c[0, 2])) for c in p]
        rotset.add(tuple(sorted(p1)))
    return rotset

def transforms(piece):
    mirror = [((4 - x), y, z) for x, y, z in piece]
    transforms = set()
    transforms.update(rotations(piece))
    transforms.update(rotations(mirror))
    return transforms

def main(argv=sys.argv):
    with open('pieces.pkl', 'rb') as f:
        pieces = pickle.load(f)
    founds = []
    with open('solutions.txt', 'r') as f:
        for line in f:
            parts = line.split()[:8]
            solution = [c for x in parts for c in pieces[int(x, 16)]]
            founds.append(transforms(solution))
    coords = list(product(xrange(MAXX), repeat=3))
    provideds = []
    for strsol in PROVIDED:
        solution = tuple(c for c, s in izip(coords, strsol) if s != " ")
        provideds.append(transforms(solution))
    for i, solutions in enumerate(provideds):
        for j, found in enumerate(founds):
            for transform in solutions:
                if transform in found:
                    print "Provided #%d is equivalent to found #%d" % (i+1, j+1)
                    break
            else:
                continue
            break
    return 0

if __name__ == '__main__':
    sys.exit(main())
