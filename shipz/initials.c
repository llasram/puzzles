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

#define PIECES_PATH "pieces.dat"
#define OCCUPATIONS_PATH "occupations.dat"
#define NVERTICES (8)

static size_t npieces = 0;
static piece_t *pieces = NULL;
static occupation_t **occupations = NULL;
static size_t vertices[NVERTICES] = { 0, };
static unsigned solution[NVERTICES] = { 0, };
static void *seen = NULL;

static void
output_map()
{
    for (int i = 0; i < NVERTICES; i++)
        printf("%04x ", solution[i]);
    printf("\n");
    return;
}

static bool
record_board(cube_t *cube)
{
    void **hbitsp = NULL;
    bool retval = 0;
    
    JLI(hbitsp, seen, cube->low);
    J1S(retval, *hbitsp, cube->high);

    return retval;
}

static void
record_isomorphs(cube_t *cube)
{
    cube_t isomorph = { 0, 0 };

    /* t = 0 is identity transform */
    for (unsigned t = 1; t < CUBE_GROUP24_TRANSFORMS; t++) {
        cube_transform_group24(&isomorph, cube, t);
        record_board(&isomorph);
    }

    return;
}

static void
recurse_vertices(cube_t *prevb, unsigned v)
{
    occupation_t *vertex = occupations[vertices[v++]];
    cube_t board = CUBE_EMPTY;

    for (size_t i = 0; i < vertex->count; i++) {
        piece_t *piece = vertex->pieces[i];
        cube_t *part = &piece->board;

        if (cube_overlap(prevb, part))
            continue;
        cube_merge(&board, prevb, part);
        if (!cube_solvable(&board, pieces, npieces))
            continue;
        if (!record_board(&board))
            continue;
        record_isomorphs(&board);

        solution[v - 1] = piece->index;
        if (v == NVERTICES) {
            output_map();
            continue;
        }

        recurse_vertices(&board, v);
    }

    return;
}

int
main(unused int argc, unused char *argv[])
{
    size_t vidx = 0;
    cube_t board = CUBE_EMPTY;

    pieces = pieces_load(PIECES_PATH, &npieces);
    if (pieces == NULL)
        return 1;
    occupations = occupations_load(OCCUPATIONS_PATH, pieces);
    if (occupations == NULL)
        return 1;
    for (unsigned x = 0; x < CUBE_MAXX; x += 4) {
        for (unsigned y = 0; y < CUBE_MAXY; y += 4) {
            for (unsigned z = 0; z < CUBE_MAXZ; z += 4) {
                vertices[vidx++] = cube_bit(x, y, z);
            }
        }
    }

    recurse_vertices(&board, 0);

    return 0;
}
