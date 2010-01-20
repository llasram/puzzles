#! /usr/bin/python

import sys
import math
from math import sin, cos
import cPickle as pickle
import struct
import numpy
from numpy import matrix, array
from itertools import izip
from support import *

BITSET_TEMPLATE = \
    "    dst->low = (dst->low & ~(1L << %d)) | (((low >> %d) & 1L) << %d);"

def build_transform_func(i, cube, pattern):
    output = [
        'inline unused cube_t *',
        'cube_transform_%d(cube_t *dst, cube_t *src)' % (i,),
        '{',
        '    static const unsigned cube_transform_pos_%d[] = {' % (i,),
        '    };',
        '    uint64_t low = src->low;',
        '    dst->pos = cube_transform_pos_%d[src->pos];' % (i,),
        '    dst->low = low;']
    tpos = list(xrange(64))
    for c1, c2 in izip(cube, pattern):
        if c1 == c2:
            continue
        src = coord_bit(*c1)
        dst = coord_bit(*c2)
        tpos[src] = dst
        output.append(BITSET_TEMPLATE % (dst, src, dst))
    output.insert(4, '        ' + ', '.join(str(p) for p in tpos))
    output.extend(['    return dst;', '}', ''])
    return '\n'.join(output)

def build_transform_group(group, isomorphs):
    output = [
        '',
        'inline unused cube_t *',
        'cube_transform_group%d(cube_t *dst, cube_t *src, int index)' % \
            (group,),
        '{',
        '    switch(index) {']
    index = 0
    for i, isomorph in enumerate(isomorphs):
        output.extend([
            '    case %d:' % (index,),
            '        return cube_transform_%d(dst, src);' % (i,)])
        index += 1
    output.insert(0, '#define CUBE_GROUP%d_TRANSFORMS (%d)' % (group, index))
    output.extend([
        '    default:',
        '        return NULL;',
        '    }',
        '}',
        ''])
    return '\n'.join(output)

def main(argv=sys.argv):
    cube = []
    mirror = []
    for x, y, z in product(xrange(MAXX), repeat=3):
        cube.append((x, y, z))
        mirror.append(((MAXX - 1) - x, y, z))
    base1 = matrix(cube)
    base2 = matrix(mirror)
    isomorphs = set()
    for x, y, z in product(ANGLES, repeat=3):
        for base in (base1, base2):
            r = (base * rot_matrix(x, y, z)).round(0)
            minr = matrix([r[:,0].min(), r[:,1].min(), r[:,2].min()])
            r -= minr
            r = tuple(tuple(int(i) for i in j) for j in r.tolist())
            isomorphs.add(r)
    isomorphs = sorted(isomorphs)
    print "#ifndef TRANSFORMS_H"
    print "#define TRANSFORMS_H\n"
    for i, pattern in enumerate(isomorphs):
        print build_transform_func(i, cube, pattern)
    print build_transform_group(24, isomorphs)
    print "#endif /* TRANSFORMS_H */"
    return 0

if __name__ == '__main__':
    sys.exit(main())
