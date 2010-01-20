#! /usr/bin/python

from __future__ import with_statement

import sys
import math
import copy
import cPickle as pickle
import struct
import numpy
from numpy import matrix, array
from collections import defaultdict
from itertools import izip
import operator
from support import *

PIECES = [
    [(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0), (0, 1, 0), (2, 1, 0),
     (0, 0, 1), (2, 0, 1)],
    [(0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0), (3, 1, 0),
     (2, 1, 1), (3, 1, 1), (4, 1, 1),
     (3, 1, 2)],
    [(1, 1, 0), (2, 1, 0), (2, 0, 0),
     (0, 1, 1), (1, 1, 1), (2, 0, 1), (3, 0, 1), (4, 0, 1),
     (0, 1, 2), (3, 0, 2)],
    [(0, 1, 0), (1, 1, 0), (1, 2, 0), (2, 2, 0), (2, 3, 0), (3, 2, 0),
     (3, 1, 0),
     (3, 1, 1), (3, 0, 1),
     (3, 0, 2), (4, 0, 2)],
    [(0, 0, 0), (0, 1, 0), (0, 2, 0), (1, 1, 0), (2, 0, 0), (2, 1, 0),
     (0, 1, 1), (2, 1, 1), (2, 2, 1), (2, 3, 1), (3, 2, 1)],
    [(0, 0, 0), (1, 0, 0), (2, 0, 0), (2, 1, 0), (2, 2, 0), (2, 3, 0),
     (3, 3, 0),
     (0, 0, 1), (3, 3, 1), (4, 3, 1),
     (2, 3, 2), (3, 3, 2)],
    [(0, 0, 0), (1, 0, 0), (3, 1, 0), (3, 2, 0), (3, 3, 0),
     (1, 0, 1), (2, 0, 1), (3, 0, 1), (3, 1, 1), (3, 3, 1),
     (1, 0, 2), (3, 3, 2),
     (1, 0, 3), (1, 1, 3)],
    [(0, 1, 0), (1, 1, 0), (2, 1, 0), (3, 1, 0), (4, 1, 0),
     (0, 1, 1), (4, 1, 1), (4, 0, 1),
     (0, 1, 2), (3, 1, 2), (4, 1, 2),
     (4, 1, 3), (4, 2, 3),
     (4, 1, 4)],
    ]

def ROTATIONS():
    rotations = []
    for piece in PIECES:
        rotset = set()
        rotlist = []
        for x, y, z in product(ANGLES, repeat=3):
            p = (piece * rot_matrix(x, y, z)).round(0)
            minp = matrix([p[:,0].min(), p[:,1].min(), p[:,2].min()])
            p -= minp
            p1 = [(int(c[0, 0]), int(c[0, 1]), int(c[0, 2])) for c in p]
            rotset.add(tuple(p1))
        rotations.append(sorted(rotset))
    rotations[0] = rotations[0][:1]
    return rotations
ROTATIONS = ROTATIONS()

def OPTIONS():
    options = [[] for p in PIECES]
    def options_at_coord(x, y, z):
        for i, pieces in enumerate(ROTATIONS):
            popts = options[i]
            for p in pieces:
                p1 = []
                for x1, y1, z1 in p:
                    x1 += x
                    y1 += y
                    z1 += z
                    c1 = (x1, y1, z1)
                    if x1 < 0 or x1 >= MAXX or \
                       y1 < 0 or y1 >= MAXY or \
                       z1 < 0 or z1 >= MAXZ or \
                       c1 in VERBOTEN:
                        break
                    p1.append((x1, y1, z1))
                else:
                    popts.append(p1)
    for x, y, z in product(xrange(MAXX), repeat=3):
        options_at_coord(x, y, z)
    return options
OPTIONS = OPTIONS()

def main(argv=sys.argv):
    with open('pieces.pkl', 'wb') as f:
        pickle.dump(OPTIONS, f, -1)
    with open('pieces.dat', 'wb') as f:
        npieces = len(PIECES)
        format = "%dP" % (npieces,)
        startdata = struct.calcsize(format)
        f.seek(startdata)
        positions = [0] * npieces
        for i, pieces in enumerate(OPTIONS):
            positions[i] = f.tell()
            f.write(struct.pack("L", len(pieces)))
            for j, piece in enumerate(pieces):
                f.write(struct.pack("L", j) + serialize_cube(cubify(piece)))
        f.seek(0)
        f.write(struct.pack(format, *positions))
    return 0

if __name__ == '__main__':
    sys.exit(main())
