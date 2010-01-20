#! /usr/bin/python

from __future__ import with_statement

import sys
import os
import struct
from support import *

def display_pieces(f):
    format = "L2Q"
    size = struct.calcsize(format)
    data = f.read(size)
    while data:
        piece = struct.unpack(format, data)
        print "%04x %016x%016x" % piece
        data = f.read(size)
    return

def main(argv=sys.argv):
    if len(argv) > 1:
        for path in argv[1:]:
            with open(path, 'rb') as f:
                display_pieces(f)
    else:
        display_pieces(stdin)
    return 0

if __name__ == '__main__':
    sys.exit(main())
