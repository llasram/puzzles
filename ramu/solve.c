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

#include "cube.h"

#define PIECES_PATH "pieces.dat"

static piece_t **pieces = NULL;
static option_t *solution[NSOLUTION] = { NULL, };

static void
output_map(void)
{
    for (size_t i = 0; i < NSOLUTION; i++) {
        option_t *option = solution[i];
        char *sep = (i == (NSOLUTION - 1) ? "\n" : " ");

        if (option == NULL) {
            printf("NULL%s", sep);
        } else {
            printf("%04lx%s", option->index, sep);
        }
    }
    fflush(stdout);
    return;
}

static void
solve(cube_t *prevb, size_t pidx)
{
    piece_t *piece = pieces[pidx];
    cube_t board = CUBE_EMPTY;

    for (size_t i = 0; i < piece->count; i++) {
        option_t *option = &piece->options[i];
        cube_t *part = &option->board;

        if (cube_overlap(prevb, part))
            continue;

        cube_merge(&board, prevb, part);
        solution[pidx] = option;
        if (pidx >= (NSOLUTION - 1)) {
            output_map();
            return;
        }

        solve(&board, pidx + 1);
    }
}

int
main(unused int argc, unused char *argv[])
{
    cube_t board = CUBE_EMPTY;

    pieces = pieces_load(PIECES_PATH);
    if (pieces == NULL)
        return 1;

    solve(&board, 0);

    return 0;
}
