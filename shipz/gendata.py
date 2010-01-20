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

PIECE = matrix([(0, 0, 0), (1, 0, 0), (2, 0, 0), (2, 1, 0), (3, 1, 0)])

PIECES = set()
for x, y, z in product(ANGLES, repeat=3):
    p = (PIECE * rot_matrix(x, y, z)).round(0)
    minp = matrix([p[:,0].min(), p[:,1].min(), p[:,2].min()])
    p -= minp
    p1 = []
    for coord in p:
        c1 = (int(coord[0, 0]), int(coord[0, 1]), int(coord[0, 2]))
        p1.append(c1)
    PIECES.add(tuple(sorted(p1)))
PIECES = sorted(PIECES)

def OPTIONS():
    options = set()
    def options_at_coord(x, y, z):
        for p in PIECES:
            p1 = []
            for x1, y1, z1 in p:
                x1 += x
                y1 += y
                z1 += z
                if x1 < 0 or x1 >= MAXX or \
                   y1 < 0 or y1 >= MAXY or \
                   z1 < 0 or z1 >= MAXZ:
                    break
                p1.append((x1, y1, z1))
            else:
                options.add(tuple(sorted(p1)))
    for x, y, z in product(xrange(MAXX), repeat=3):
        options_at_coord(x, y, z)
    return sorted(options)
OPTIONS = OPTIONS()

def main(argv=sys.argv):
    base = array([[[0]*MAXZ for i in xrange(MAXY)] for j in xrange(MAXX)])
    cubes = [None] * len(OPTIONS)
    occupation = defaultdict(set)
    for i, piece in enumerate(OPTIONS):
        cube = base.copy()
        for x, y, z in piece:
            cube[x, y, z] = 1
            occupation[coord_bit(x, y, z)].add(i)
        cubes[i] = cube
    with open('pieces.pkl', 'wb') as f:
        pickle.dump(OPTIONS, f, -1)
    with open('pieces.dat', 'wb') as f:
        for i, cube in enumerate(cubes):
            f.write(struct.pack('L', i) + serialize_cube(cube))
    with open('occupations.dat', 'wb') as f:
        startdata = struct.calcsize("125P")
        f.seek(startdata)
        positions = [0] * 125
        for bit in xrange(125):
            positions[bit] = f.tell()
            indices = occupation[bit]
            f.write(struct.pack("L", len(indices)))
            for idx in indices:
                f.write(struct.pack("P", idx))
        f.seek(0)
        f.write(struct.pack("125P", *positions))
    return 0

if __name__ == '__main__':
    sys.exit(main())
