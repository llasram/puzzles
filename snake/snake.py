#! /usr/bin/python

import sys
import os
import copy

PATTERN = [ 2, 4,
            2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 2, 4, 2, 2, 2, 4, 3,
            2, 2, 2, 2, 2, 3, 3,
            2, 2, 2, 2, 2, 2, 2, 2,
            3, 2, 3, 2, 3, 2, 4,
            2, 2, 3, 2, 3 ]

def print_snake():
    right = 0
    godown = True
    for item in PATTERN:
        item -= 1
        if godown:
            for i in xrange(item - 1):
                print "%s*" % (" " * right,)
        else:
            print "%s%s" % (" " * right, "*" * (item + 1))
            right += item
        godown = not godown
    return

class Position(object):
    def __init__(self, pos, y=None, z=None):
        if y is not None or z is not None:
            pos = [pos, y, z]
        self.pos = pos

    def __repr__(self):
        return "Position(%r)" % (self.pos,)
        
    def __add__(self, other):
        pos = [a + b for a, b in zip(self.pos, other.pos)]
        return Position(pos)

    def __iadd__(self, other):
        self.pos = [a + b for a, b in zip(self.pos, other.pos)]
        return self
    
    def __mul__(self, other):
        pos = [a * other for a in self.pos]
        return Position(pos)

    def check_cube(self, cube):
        val = cube[self.pos[0]][self.pos[1]][self.pos[2]]
        cube[self.pos[0]][self.pos[1]][self.pos[2]] = True
        return val

    def in_cube(self):
        for coord in self.pos:
            if coord < 0 or coord > 3:
                return False
        return True

LEFT = Position(-1, 0, 0)
RIGHT = Position(1, 0, 0)
UP = Position(0, 1, 0)
DOWN = Position(0, -1, 0)
FORWARD = Position(0, 0, 1)
BACK = Position(0, 0, -1)
DIRS = [LEFT, RIGHT, UP, DOWN, FORWARD, BACK]
OPPOSE = { LEFT: RIGHT, RIGHT: LEFT, UP: DOWN, DOWN: UP,
           BACK: FORWARD, FORWARD: BACK, None: None }
RADIRS = dict((d, [rad for rad in DIRS if rad is not d and rad is not OPPOSE[rad]]) for d in DIRS)
IDIRS = { LEFT: 'left', RIGHT: 'right', UP: 'up', DOWN: 'down',
          FORWARD: 'forward', BACK: 'back' }

def solve(cube, idx, pos, ldir=None):
    if idx == len(PATTERN):
        return []
    item = PATTERN[idx]
    for d in RADIRS.get(ldir, DIRS):
        npos = copy.copy(pos)
        step = copy.deepcopy(cube)
        for i in xrange(item - 1):
            npos += d
            if not npos.in_cube() or npos.check_cube(step):
                break
        else:
            sol = solve(step, idx + 1, npos, d)
            if sol is not None:
                sol.insert(0, (d, item))
                return sol
    return None

def main(argv=sys.argv):
    empty = [[[False]*4 for i in xrange(4)] for j in xrange(4)]
    for i in xrange(2):
        for j in xrange(2):
            for k in xrange(2):
                cube = copy.deepcopy(empty)
                cube[i][j][k] = True
                pos = Position(i, j, k)
                sol = solve(cube, 0, pos)
                if sol is not None:
                    print (i, j, k)
                    for d, item in sol:
                        print IDIRS[d], item
                    return 0
    return 0

if __name__ == '__main__':
    sys.exit(main())
