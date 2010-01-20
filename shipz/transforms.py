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
    "    %s = (%s & ~(1L << %d)) | (((%s >> %d) & 1L) << %d);"

def build_transform_func(i, cube, pattern):
    output = [
        'inline unused cube_t *',
        'cube_transform_%i(cube_t *dst, cube_t *src)' % (i,),
        '{',
        '    uint64_t low = src->low;',
        '    uint64_t high = src->high;',
        '    dst->low = low;',
        '    dst->high = high;']
    for c1, c2 in izip(cube, pattern):
        if c1 == c2:
            continue
        src = coord_bit(*c1)
        if src < 64:
            srcs = "low"
        else:
            srcs = "high"
            src -= 64
        dst = coord_bit(*c2)
        if dst < 64:
            dsts = "dst->low"
        else:
            dsts = "dst->high"
            dst -= 64
        output.append(BITSET_TEMPLATE % \
                          (dsts, dsts, dst, srcs, src, dst))
    output.extend(['    return dst;', '}', ''])
    return '\n'.join(output)

def build_transform_group(group, isomorphs, stable, refs=None):
    output = [
        '',
        'inline unused cube_t *',
        'cube_transform_group%d(cube_t *dst, cube_t *src, int index)' % \
            (group,),
        '{',
        '    switch(index) {']
    cube = list(isomorphs[0])
    stable = cube.index(stable)
    index = 0
    for i, isomorph in enumerate(isomorphs):
        if cube[stable] != isomorph[stable]:
            continue
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
    if refs:
        ref1s = cube.index(refs[0])
        ref1d = cube.index(refs[1])
        ref2s = cube.index(refs[2])
        ref2d = cube.index(refs[3])
        for i, isomorph in enumerate(isomorphs):
            if cube[ref1s] != isomorph[ref1d] or \
               cube[ref2s] != isomorph[ref2d]:
                continue
            output.extend([
                '#define cube_reflect_group%d cube_transform_%d' % (group, i),
                ''])
            break
    return '\n'.join(output)

def main(argv=sys.argv):
    shift = matrix([2, 2, 2])
    cube = []
    mirror = []
    for x, y, z in product(xrange(MAXX), repeat=3):
        cube.append((x, y, z))
        mirror.append(((MAXX - 1) - x, y, z))
    base1 = matrix(cube) - shift
    base2 = matrix(mirror) - shift
    isomorphs = set()
    for x, y, z in product(ANGLES, repeat=3):
        for base in (base1, base2):
            r = (base * rot_matrix(x, y, z)).round(0)
            r += shift
            r = tuple(tuple(int(i) for i in j) for j in r.tolist())
            isomorphs.add(r)
    isomorphs = sorted(isomorphs)
    print "#ifndef TRANSFORMS_H"
    print "#define TRANSFORMS_H\n"
    for i, pattern in enumerate(isomorphs):
        print build_transform_func(i, cube, pattern)
    print build_transform_group(3, isomorphs, (0, 0, 0),
                                [(0, 2, 0), (0, 2, 4), (4, 2, 0), (4, 2, 4)])
    print build_transform_group(6, isomorphs, (0, 0, 2),
                                [(0, 0, 2), (4, 0, 2), (0, 4, 2), (4, 4, 2)])
    print build_transform_group(12, isomorphs, (2, 0, 2),
                                [(0, 0, 2), (0, 4, 2), (4, 0, 2), (4, 4, 2)])
    print build_transform_group(24, isomorphs, (2, 2, 2))
    print "#endif /* TRANSFORMS_H */"
    return 0

if __name__ == '__main__':
    sys.exit(main())
