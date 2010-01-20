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

def rotations(piece):
    rotset = set()
    for x, y, z in product(ANGLES, repeat=3):
        p = (piece * rot_matrix(x, y, z)).round(0)
        minp = matrix([p[:,0].min(), p[:,1].min(), p[:,2].min()])
        p -= minp
        p1 = [(int(c[0, 0]), int(c[0, 1]), int(c[0, 2])) for c in p]
        rotset.add(tuple(p1))
    return rotset

def transforms(piece):
    mirror = [((4 - x), y, z) for x, y, z in piece]
    transforms = set()
    transforms.update(rotations(piece))
    transforms.update(rotations(mirror))
    return transforms

def main(argv=sys.argv):
    with open('positions.pkl', 'rb') as f:
        positions = pickle.load(f)
    founds = []
    with open('solutions.txt', 'r') as f:
        for line in f:
            solution = [int(x) for x in line.split()]
            solution = matrixize(positions, solution)
            founds.append(transforms(solution))
    coords = list(product(xrange(MAXX), repeat=3))
    for i, solutions in enumerate(founds):
        for j, found in enumerate(founds):
            for transform in solutions:
                if transform in found:
                    print "Found #%d is equivalent to found #%d" % (i+1, j+1)
                    break
            else:
                continue
            break
    return 0

if __name__ == '__main__':
    sys.exit(main())
