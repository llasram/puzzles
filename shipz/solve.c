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
#define OCCUPATIONS_PATH "occupations.dat"
#define NVERTICES (8)

static size_t npieces = 0;
static piece_t *pieces = NULL;
static occupation_t **occupations = NULL;
static piece_t *solution[NSOLUTION] = { NULL, };

#define TIMER_INTERVAL (10)

static bool timer_signaled = true;
time_t timer_expected = 0;

void
handle_sigalarm(unused int signo)
{
    timer_signaled = true;
}

static bool
timer_setup(void)
{
    int rv = 0;
    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_RESTART;
    rv = sigemptyset(&act.sa_mask);
    if (rv < 0) {
        perror("sigemptyset()");
        return false;
    }
    act.sa_handler = &handle_sigalarm;
    rv = sigaction(SIGALRM, &act, NULL);
    if (rv < 0) {
        perror("sigaction()");
        return false;
    }
    timer_expected = time(NULL);

    return true;
}

static void
timer_reset(void)
{
    time_t now = time(NULL);
    time_t next = timer_expected + TIMER_INTERVAL;
    time_t interval =  next - now;

    timer_signaled = false;
    timer_expected = next;
    alarm(interval);

    return;
}

static void
output_map(void)
{
    for (size_t i = 0; i < NSOLUTION; i++) {
        piece_t *piece = solution[i];
        char *sep = i == 7 ? "\t" : (i == 24 ? "\n" : " ");

        if (piece == NULL) {
            printf("NULL%s", sep);
        } else {
            printf("%04lx%s", piece->index, sep);
        }
    }
    fflush(stdout);
    return;
}

static void
solve(cube_t *prevb, size_t bit, size_t count)
{
    occupation_t *occupy = NULL;
    cube_t board = CUBE_EMPTY;

    while (cube_getbit(prevb, bit))
        bit++;
    occupy = occupations[bit];

    for (size_t i = 0; i < occupy->count; i++) {
        piece_t *piece = occupy->pieces[i];
        cube_t *part = &piece->board;

        if (cube_overlap(prevb, part))
            continue;

        cube_merge(&board, prevb, part);
        solution[count] = piece;
        if (count >= (NSOLUTION - 1)) {
            output_map();
            return;
        }

        if (!cube_solvable(&board, pieces, npieces))
            continue;
        solve(&board, bit + 1, count + 1);
    }
}

static bool
parse_state(cube_t *board, char *line)
{
    char *cur = line;
    char *end = NULL;
    unsigned long index = 0;
    piece_t *piece = NULL;

    cube_clear(board);
    
    for (int i = 0; i < NVERTICES; i++) {
        index = strtoul(cur, &end, 16);
        if (end != (cur + 4) || index > npieces)
            return false;
        piece = &pieces[index];
        solution[i] = piece;
        cube_merge(board, board, &piece->board);
        cur = end + 1;
    }

    return true;
}

int
main(int argc, char *argv[])
{
    cube_t board = CUBE_EMPTY;
    char line[128] = { 0, };
    FILE *input = stdin;
    unsigned count = 0;

    pieces = pieces_load(PIECES_PATH, &npieces);
    if (pieces == NULL)
        return 1;
    occupations = occupations_load(OCCUPATIONS_PATH, pieces);
    if (occupations == NULL)
        return 1;

    if (argc > 1) {
        char *path = argv[1];
        input = fopen(path, "r");
        if (input == NULL) {
            perror(path);
            return 1;
        }
    }

    if (!timer_setup())
        return 1;

    while (fgets(line, sizeof(line), input)) {
        count++;
        if (timer_signaled) {
            fprintf(stderr, "info: solving #%05d: %s", count, line);
            timer_reset();
        }

        if (!parse_state(&board, line)) {
            fprintf(stderr, "error: could not parse input line\n");
            return 1;
        }

        solve(&board, 0, NVERTICES);
    }

    return 0;
}
