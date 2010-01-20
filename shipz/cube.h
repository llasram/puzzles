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

#define NSOLUTION (25)

typedef struct cube_t {
    uint64_t low;
    uint64_t high;
} cube_t;

typedef struct piece_t {
    size_t index;
    cube_t board;
} piece_t;

typedef struct occupation_t {
    size_t count;
    piece_t *pieces[];
} occupation_t;

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
cube_filled(cube_t *cube) 
{
    return ((~cube->low == 0) && (cube->high == CUBE_HIGH_FILL));
}

inline unused bool
cube_empty(cube_t *cube) 
{
    return ((cube->low == 0) && (cube->high == 0));
}

inline unused bool
cube_solvable(cube_t *cube, piece_t *pieces, size_t npieces)
{
    cube_t gapcheck = CUBE_EMPTY;
    
    cube_copy(&gapcheck, cube);
    for (size_t i = 0; i < npieces; i++) {
        cube_t *piece = &pieces[i].board;
        if (!cube_overlap(cube, piece)) {
            cube_merge(&gapcheck, &gapcheck, piece);
        }
    }

    return cube_filled(&gapcheck);
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

static unused piece_t *
pieces_load(const char *path, size_t *npieces)
{
    int rv = -1;
    int fd = -1;
    piece_t *pieces = NULL;
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
    pieces = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (pieces == (void *)-1) {
        perror(path);
        return NULL;
    }
    *npieces = buf.st_size / sizeof(*pieces);

    return pieces;
}

static unused occupation_t **
occupations_load(const char *path, piece_t *pieces)
{
    int rv = -1;
    int fd = -1;
    int prot = 0;
    occupation_t **occupations = NULL;
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
    occupations = mmap(NULL, buf.st_size, prot, MAP_PRIVATE, fd, 0);
    if (occupations == (void *)-1) {
        perror(path);
        return NULL;
    }
    for (size_t i = 0; i < CUBE_BITS; i++) {
        occupation_t *occupation = NULL;
        occupations[i] = (occupation_t *)((uint8_t *)occupations +
                                          (size_t)occupations[i]);
        occupation = occupations[i];
        for (size_t j = 0; j < occupation->count; j++) {
            occupation->pieces[j] = &pieces[(size_t)occupation->pieces[j]];
        }
    }

    return occupations;
}

#include "transforms.h"

#endif /* CUBE_H */
