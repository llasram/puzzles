import struct
import numpy
from numpy import matrix, array
import math
from math import sin, cos

ANGLES = [0, math.pi * 0.5, math.pi, math.pi * 1.5]

MAXX = 5
MAXY = 5
MAXZ = 5

VERBOTEN = [
    (0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1),
    (4, 0, 0), (3, 0, 0), (4, 1, 0), (4, 0, 1),
    (0, 4, 0), (1, 4, 0), (0, 3, 0), (0, 4, 1),
    (4, 4, 0), (3, 4, 0), (4, 3, 0), (4, 4, 1),
    (0, 0, 4), (1, 0, 4), (0, 1, 4), (0, 0, 3),
    (4, 0, 4), (3, 0, 4), (4, 1, 4), (4, 0, 3),
    (0, 4, 4), (1, 4, 4), (0, 3, 4), (0, 4, 3),
    (4, 4, 4), (3, 4, 4), (4, 3, 4), (4, 4, 3),
    ]

def serialize_cube11(cube):
    key = long(0)
    for bit in cube.flat:
        key = (key << 1) | long(bit)
    return key

def serialize_cube1(cube):
    key = serialize_cube11(cube)
    low = key & 0xffffffffffffffff
    high = (key >> 64) & 0xffffffffffffffff
    return (low, high)

def serialize_cube(cube):
    low, high = serialize_cube1(cube)
    return struct.pack('QQ', low, high)

def deserialize_cube(c, c2=None):
    if c2 != None:
        c = c | (c2 << 64)
    bits = [(c >> i) & 1 for i in xrange(124, -1, -1)]
    return array(bits).reshape(5, 5, 5)

def coord_bit(x, y, z):
    return 124 - ((x * 25) + (y * 5) + z)

def coord_unbit(b):
    b = 124 - b
    x, b = b / 25, b % 25
    y, b = b / 5, b % 5
    z = b
    return (x, y, z)

def get_coord(c, x, y, z):
    return (c >> coord_bit(x, y, z)) & 1

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

def product(*args, **kwds):
    # product('ABCD', 'xy') --> Ax Ay Bx By Cx Cy Dx Dy
    # product(range(2), repeat=3) --> 000 001 010 011 100 101 110 111
    pools = map(tuple, args) * kwds.get('repeat', 1)
    result = [[]]
    for pool in pools:
        result = [x+[y] for x in result for y in pool]
    for prod in result:
        yield tuple(prod)
