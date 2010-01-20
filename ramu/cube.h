#ifndef CUBE_H
#define CUBE_H

#include <stdint.h>
#include <stdbool.h>

#define unused __attribute__((__unused__))

#define CUBE_MAXX (5)
#define CUBE_MAXY (5)
#define CUBE_MAXZ (5)

#define CUBE_BITS (125)
#define CUBE_HIGH_FILL ((1L << (CUBE_BITS - 64)) - 1)
#define CUBE_EMPTY { 0, 0 }

#define NSOLUTION (8)

typedef struct cube_t {
    uint64_t low;
    uint64_t high;
} cube_t;

typedef struct option_t {
    size_t index;
    cube_t board;
} option_t;

typedef struct piece_t {
    size_t count;
    option_t options[];
} piece_t;

inline unused unsigned int
cube_bit(unsigned x, unsigned y, unsigned z) 
{
    return (124 - ((x * 25) + (y * 5) + z));
}

inline unused cube_t *
cube_clear(cube_t *cube) 
{
    cube->low = 0;
    cube->high = 0;
    return cube;
}

inline unused cube_t *
cube_copy(cube_t *dst, cube_t *src)
{
    dst->low = src->low;
    dst->high = src->high;
    return dst;
}

inline unused cube_t *
cube_copy_invert(cube_t *dst, cube_t *src)
{
    dst->low = ~src->low;
    dst->high = ~src->high & ((1L << (125 - 64)) - 1);
    return dst;
}

inline unused cube_t *
cube_invert(cube_t *cube)
{
    cube->low = ~cube->low;
    cube->high = ~cube->high & ((1L << (125 - 64)) - 1);
    return cube;
}

inline unused bool
cube_equal(cube_t *cube1, cube_t *cube2) 
{
    return (cube1->low == cube2->low && cube1->high == cube1->high);
}

inline unused bool
cube_lessthan(cube_t *cube1, cube_t *cube2)
{
    return ((cube1->high < cube2->high ||
            (cube1->high == cube2->high &&
             cube1->low < cube2->low)));
}

inline unused bool
cube_overlap(cube_t *cube1, cube_t *cube2) 
{
    return ((cube1->high & cube2->high) || (cube1->low & cube2->low));
}

inline unused cube_t *
cube_merge3(cube_t *dst, cube_t *src1, cube_t *src2, cube_t *src3)
{
    dst->low = src1->low | src2->low | src3->low;
    dst->high = src1->high | src2->high | src3->high;
    return dst;
}

inline unused cube_t *
cube_merge(cube_t *dst, cube_t *src1, cube_t *src2)
{
    dst->low = src1->low | src2->low;
    dst->high = src1->high | src2->high;
    return dst;
}

inline unused cube_t *
cube_subtract(cube_t *dst, cube_t *src1, cube_t *src2)
{
    dst->low = src1->low & ~src2->low;
    dst->high = src1->high & ~src2->high;
    return dst;
}

inline unused bool
cube_match(cube_t *cube, cube_t *pattern)
{
    return (((cube->high & pattern->high) == pattern->high) &&
            ((cube->low & pattern->low) == pattern->low));
}

inline unused bool
cube_any(cube_t *cube, cube_t *pattern) 
{
    return ((cube->high & pattern->high) || (cube->low & pattern->low));
}

inline unused bool
cube_filled(cube_t *cube) 
{
    return ((~cube->low == 0) && (cube->high == CUBE_HIGH_FILL));
}

inline unused bool
cube_empty(cube_t *cube) 
{
    return ((cube->low == 0) && (cube->high == 0));
}

inline unused int
cube_get(cube_t *cube, int x, int y, int z) 
{
    unsigned int bit = cube_bit(x, y, z);

    if (bit < 64) {
        return (cube->low >> bit) & 1;
    } else {
        return (cube->high >> (bit - 64)) & 1;
    }
}

inline unused int
cube_getbit(cube_t *cube, unsigned bit) 
{
    if (bit < 64) {
        return (cube->low >> bit) & 1;
    } else {
        return (cube->high >> (bit - 64)) & 1;
    }
}

inline unused int
cube_set(cube_t *cube, unsigned x, unsigned y, unsigned z, bool v)
{
    unsigned int bit = cube_bit(x, y, z);

    if (bit < 64) {
        cube->low = (cube->low & ~(1L << bit)) | ((uint64_t)v << bit);
    } else {
        bit -= 64;
        cube->high = (cube->high & ~(1L << bit)) | ((uint64_t)v << bit);
    }
    return v;
}

static unused piece_t **
pieces_load(const char *path)
{
    int rv = -1;
    int fd = -1;
    int prot = 0;
    piece_t **pieces = NULL;
    struct stat buf;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror(path);
        return NULL;
    }
    rv = fstat(fd, &buf);
    if (rv < 0) {
        perror(path);
        return NULL;
    }
    prot = PROT_READ | PROT_WRITE;
    pieces = mmap(NULL, buf.st_size, prot, MAP_PRIVATE, fd, 0);
    if (pieces == (void *)-1) {
        perror(path);
        return NULL;
    }
    for (size_t i = 0; i < NSOLUTION; i++) {
        pieces[i] = (piece_t *)((uint8_t *)pieces + (size_t)pieces[i]);
    }

    return pieces;
}

#include "transforms.h"

#endif /* CUBE_H */
