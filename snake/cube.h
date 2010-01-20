#ifndef CUBE_H
#define CUBE_H

#include <stdint.h>
#include <stdbool.h>

#define unused __attribute__((__unused__))

#define CUBE_MAXX (4)
#define CUBE_MAXY (4)
#define CUBE_MAXZ (4)

#define CUBE_BITS (64)
#define CUBE_EMPTY { 0, 0 }

typedef struct cube_t {
    uint64_t low;
    uint64_t pos;
} cube_t;

typedef struct option_t {
    cube_t board;
    uint64_t dir;
} option_t;

typedef struct piece_t {
    size_t count;
    option_t options[];
} piece_t;

typedef piece_t *position_t[3];

inline unused unsigned int
cube_bit(unsigned x, unsigned y, unsigned z) 
{
    return ((CUBE_BITS-1) - ((x * 16) + (y * 4) + z));
}

inline unused cube_t *
cube_clear(cube_t *cube) 
{
    cube->low = 0;
    return cube;
}

inline unused cube_t *
cube_copy(cube_t *dst, cube_t *src)
{
    dst->low = src->low;
    dst->pos = src->pos;
    return dst;
}

inline unused cube_t *
cube_copy_invert(cube_t *dst, cube_t *src)
{
    dst->low = ~src->low;
    return dst;
}

inline unused cube_t *
cube_invert(cube_t *cube)
{
    cube->low = ~cube->low;
    return cube;
}

inline unused bool
cube_equal(cube_t *cube1, cube_t *cube2) 
{
    return (cube1->low == cube2->low);
}

inline unused bool
cube_lessthan(cube_t *cube1, cube_t *cube2)
{
    return (cube1->low < cube2->low);
}

inline unused bool
cube_overlap(cube_t *cube1, cube_t *cube2) 
{
    return ((cube1->low & cube2->low) != 0);
}

inline unused cube_t *
cube_merge3(cube_t *dst, cube_t *src1, cube_t *src2, cube_t *src3)
{
    dst->low = src1->low | src2->low | src3->low;
    dst->pos = src3->pos;
    return dst;
}

inline unused cube_t *
cube_merge(cube_t *dst, cube_t *src1, cube_t *src2)
{
    dst->low = src1->low | src2->low;
    dst->pos = src2->pos;
    return dst;
}

inline unused cube_t *
cube_subtract(cube_t *dst, cube_t *src1, cube_t *src2)
{
    dst->low = src1->low & ~src2->low;
    return dst;
}

inline unused bool
cube_match(cube_t *cube, cube_t *pattern)
{
    return ((cube->low & pattern->low) == pattern->low);
}

inline unused bool
cube_any(cube_t *cube, cube_t *pattern) 
{
    return ((cube->low & pattern->low) != 0);
}

inline unused bool
cube_filled(cube_t *cube) 
{
    return (~cube->low == 0);
}

inline unused bool
cube_empty(cube_t *cube) 
{
    return (cube->low == 0);
}

inline unused int
cube_get(cube_t *cube, int x, int y, int z) 
{
    unsigned int bit = cube_bit(x, y, z);
    return (cube->low >> bit) & 1;
}

inline unused int
cube_getbit(cube_t *cube, unsigned bit) 
{
    return (cube->low >> bit) & 1;
}

inline unused int
cube_set(cube_t *cube, unsigned x, unsigned y, unsigned z, bool v)
{
    unsigned int bit = cube_bit(x, y, z);
    cube->low = (cube->low & ~(1L << bit)) | ((uint64_t)v << bit);
    return v;
}

inline unused int
cube_setbit(cube_t *cube, unsigned bit, bool v)
{
    cube->low = (cube->low & ~(1L << bit)) | ((uint64_t)v << bit);
    return v;
}

static unused position_t *
positions_load(const char *path)
{
    int rv = -1;
    int fd = -1;
    int prot = 0;
    piece_t **pieces = NULL;
    position_t *positions = NULL;
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
    positions = mmap(NULL, buf.st_size, prot, MAP_PRIVATE, fd, 0);
    if (positions == (void *)-1) {
        perror(path);
        return NULL;
    }
    pieces = (piece_t **)positions;
    for (size_t i = 0; i < (CUBE_BITS * 3); i++) {
        pieces[i] = (piece_t *)((uint8_t *)pieces + (size_t)pieces[i]);
    }

    return positions;
}

#include "transforms.h"

#endif /* CUBE_H */
