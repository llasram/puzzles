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

UP = 0
DOWN = 1
LEFT = 2
RIGHT = 3
FORWARD = 4
BACK = 5

DIRECTIONS = list(xrange(6))

def translate(x, y, z, n, d):
    if d == UP:
        return (x, y, z + n)
    if d == DOWN:
        return (x, y, z - n)
    if d == LEFT:
        return (x - n, y, z)
    if d == RIGHT:
        return (x + n, y, z)
    if d == FORWARD:
        return (x, y + n, z)
    if d == BACK:
        return (x, y - n, z)
    return None

def OPTIONS():
    options = [None] * CUBE_BITS
    for x, y, z in product(xrange(MAXX), repeat=3):
        atpos = [[] for i in xrange(3)]
        for dir_ in DIRECTIONS:
            option = []
            x1, y1, z1 = x, y, z
            for len_ in xrange(3):
                x1, y1, z1 = translate(x1, y1, z1, 1, dir_)
                if x1 < 0 or x1 >= MAXX or \
                   y1 < 0 or y1 >= MAXY or \
                   z1 < 0 or z1 >= MAXZ:
                    break
                option.append((x1, y1, z1))
                datum = (dir_, coord_bit(x1, y1, z1), tuple(option))
                atpos[len_].append(datum)
        options[coord_bit(x, y, z)] = atpos
    return options
OPTIONS = OPTIONS()

def main(argv=sys.argv):
    with open('positions.pkl', 'wb') as f:
        pickle.dump(OPTIONS, f, -1)
    with open('positions.dat', 'wb') as f:
        noptions = CUBE_BITS * 3
        format = "%dP" % (noptions,)
        startdata = struct.calcsize(format)
        f.seek(startdata)
        positions = [[0]*3 for i in xrange(CUBE_BITS)]
        for i, options in enumerate(OPTIONS):
            lengths = positions[i]
            for j, len_ in enumerate(xrange(3)):
                lengths[j] = f.tell()
                pieces = options[j]
                f.write(struct.pack("L", len(pieces)))
                for dir_, dest, option in pieces:
                    f.write(serialize_cube(cubify(option)) +
                            struct.pack("LL", dest, dir_))
        f.seek(0)
        for lengths in positions:
            f.write(struct.pack("3L", *lengths))
    return 0

if __name__ == '__main__':
    sys.exit(main())
