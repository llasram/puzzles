import struct
import math
from math import sin, cos
from itertools import izip
import numpy
from numpy import matrix, array

MAXX = 4
MAXY = 4
MAXZ = 4

CUBE_BITS = 64

PATTERN = [ 2, 4,
            2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 2, 4, 2, 2, 2, 4, 3,
            2, 2, 2, 2, 2, 3, 3,
            2, 2, 2, 2, 2, 2, 2, 2,
            3, 2, 3, 2, 3, 2, 4,
            2, 2, 3, 2, 3 ]

def serialize_cube1(cube):
    key = long(0)
    for bit in cube.flat:
        key = (key << 1) | long(bit)
    return key

def serialize_cube(cube):
    return struct.pack('Q', serialize_cube1(cube))

def deserialize_cube(c):
    bits = [(c >> i) & 1 for i in xrange(CUBE_BITS-1, -1, -1)]
    return array(bits).reshape(MAXX, MAXY, MAXZ)

def coord_bit(x, y, z):
    return (CUBE_BITS-1) - ((x * 16) + (y * 4) + z)

def coord_unbit(b):
    b = (CUBE_BITS-1) - b
    x, b = b / 16, b % 16
    y, b = b / 4, b % 4
    z = b
    return (x, y, z)

def get_coord(c, x, y, z):
    return (c >> coord_bit(x, y, z)) & 1

ANGLES = [0, math.pi * 0.5, math.pi, math.pi * 1.5]
def rot_matrix(x, y, z):
    rotx = matrix([[1, 0, 0], [0, cos(x), sin(x)], [0, -sin(x), cos(x)]])
    roty = matrix([[cos(y), 0, -sin(y)], [0, 1, 0], [sin(y), 0, cos(y)]])
    rotz = matrix([[cos(z), sin(z), 0], [-sin(z), cos(z), 0], [0, 0, 1]])
    return rotx * roty * rotz

_CUBE = array([[[0]*MAXZ for i in xrange(MAXY)] for j in xrange(MAXX)])
def cubify(coords):
    cube = _CUBE.copy()
    for x, y, z in coords:
        cube[x, y, z] = 1
    return cube

def matrixize(positions, solution):
    result = []
    pos = solution[0]
    dirs = solution[1:]
    result.append(coord_unbit(pos))
    for dir_, size in izip(dirs, PATTERN):
        options = positions[pos][size - 2]
        for odir, dest, cube in options:
            if dir_ != odir:
                continue
            pos = dest
            result.extend(cube)
            break
    return tuple(result)

def ordered_cubify(matrix):
    cube = _CUBE.copy()
    for i, (x, y, z) in enumerate(matrix):
        cube[x, y, z] = i
    return cube

def product(*args, **kwds):
    # product('ABCD', 'xy') --> Ax Ay Bx By Cx Cy Dx Dy
    # product(range(2), repeat=3) --> 000 001 010 011 100 101 110 111
    pools = map(tuple, args) * kwds.get('repeat', 1)
    result = [[]]
    for pool in pools:
        result = [x+[y] for x in result for y in pool]
    for prod in result:
        yield tuple(prod)
