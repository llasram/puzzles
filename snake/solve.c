#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <Judy.h>
#include "cube.h"

#define POSITIONS_PATH "positions.dat"

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define FORWARD 4
#define BACK 5
#define NONE 6

static const unsigned opposite[] = {
    DOWN, UP, RIGHT, LEFT, BACK, FORWARD, NONE
};

static const unsigned pattern[] = {
    2, 4,
    2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 2, 4, 2, 2, 2, 4, 3,
    2, 2, 2, 2, 2, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 2, 3, 2, 3, 2, 4,
    2, 2, 3, 2, 3
};

#define NSOLUTION (sizeof(pattern) / sizeof(*pattern))

static position_t *positions = NULL;
static unsigned initial = 0;
static unsigned solution[NSOLUTION] = { NONE, };
void *seen = NULL;

static void
output_map(void)
{
    printf("%u\t", initial);
    for (size_t i = 0; i < NSOLUTION; i++) {
        char *sep = (i == (NSOLUTION - 1) ? "\n" : " ");
        printf("%u%s", solution[i], sep);
    }
    fflush(stdout);
    return;
}

static bool
record_board(cube_t *cube)
{
    void **hbitsp = NULL;
    bool retval = 0;
    
    JLI(hbitsp, seen, cube->low);
    J1S(retval, *hbitsp, cube->pos);

    return retval;
}

static unused void
record_isomorphs(cube_t *cube)
{
    cube_t isomorph = CUBE_EMPTY;

    /* t = 0 is identity transform */
    for (unsigned t = 1; t < CUBE_GROUP24_TRANSFORMS; t++) {
        cube_transform_group24(&isomorph, cube, t);
        record_board(&isomorph);
    }

    return;
}

static void
solve__(cube_t *prevb, unsigned pidx, unsigned dir)
{
    unsigned size = pattern[pidx];
    piece_t *piece = positions[prevb->pos][size - 2];
    cube_t board = CUBE_EMPTY;

    for (size_t i = 0; i < piece->count; i++) {
        option_t *option = &piece->options[i];
        cube_t *part = &option->board;
        unsigned ndir = option->dir;

        if (ndir == dir || ndir == opposite[dir])
            continue;
        if (cube_overlap(prevb, part))
            continue;
        cube_merge(&board, prevb, part);
        solution[pidx] = ndir;

        if (pidx >= (NSOLUTION - 1)) {
            output_map();
            return;
        } else if (pidx < 6) {
            if (!record_board(&board))
                continue;
            record_isomorphs(&board);
        }

        solve__(&board, pidx + 1, ndir);
    }
}

static void
solve(unsigned x, unsigned y, unsigned z)
{
    cube_t board = CUBE_EMPTY;
    unsigned pos = cube_bit(x, y, z);

    board.pos = initial = pos;
    cube_setbit(&board, pos, 1);
    solve__(&board, 0, NONE);

    return;
}

int
main(unused int argc, unused char *argv[])
{
    positions = positions_load(POSITIONS_PATH);
    if (positions == NULL)
        return 1;

    for (unsigned x = 0; x < 2; x++) {
        for (unsigned y = 0; y < 2; y++) {
            for (unsigned z = 0; z < 2; z++) {
                solve(x, y, z);
            }
        }
    }

    return 0;
}
