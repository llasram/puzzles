#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include "cube.h"

static bool
test_group3_transforms(void)
{
    cube_t src = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 3; z++) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int t = 0; t < CUBE_GROUP3_TRANSFORMS; t++) {
        cube_transform_group3(&dst, &src, t);
        if (!cube_equal(&dst, &src)) {
            return false;
        }
    }

    return true;
}

static bool
test_group3_reflect(void)
{
    cube_t src = { 0, 0 };
    cube_t ref = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 3; z++) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 2; z < 5; z++) {
                cube_set(&ref, x, y, z, 1);
            }
        }
    }

    cube_reflect_group3(&dst, &src);
    if (!cube_equal(&dst, &ref)) {
        printf("0x%016lx%016lx -> 0x%016lx%016lx\n",
               src.high, src.low, dst.high, dst.low);
        return false;
    }

    return true;
}

static bool
test_group6_transforms(void)
{
    cube_t src = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 5; z++) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int t = 0; t < CUBE_GROUP6_TRANSFORMS; t++) {
        cube_transform_group6(&dst, &src, t);
        if (!cube_equal(&dst, &src)) {
            printf("%d: 0x%016lx%016lx -> 0x%016lx%016lx\n", t,
                  src.high, src.low, dst.high, dst.low);
            return false;
        }
    }

    return true;
}

static bool
test_group6_reflect(void)
{
    cube_t src = { 0, 0 };
    cube_t ref = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 5; z++) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int x = 2; x < 5; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 5; z++) {
                cube_set(&ref, x, y, z, 1);
            }
        }
    }

    cube_reflect_group6(&dst, &src);
    if (!cube_equal(&dst, &ref)) {
        printf("0x%016lx%016lx -> 0x%016lx%016lx\n",
               src.high, src.low, dst.high, dst.low);
        return false;
    }

    return true;
}

static bool
test_group12_transforms(void)
{
    cube_t src = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 5; z++) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int t = 0; t < CUBE_GROUP12_TRANSFORMS; t++) {
        cube_transform_group12(&dst, &src, t);
        if (!cube_equal(&dst, &src)) {
            printf("%d: 0x%016lx%016lx -> 0x%016lx%016lx\n", t,
                  src.high, src.low, dst.high, dst.low);
            return false;
        }
    }

    return true;
}

static bool
test_group12_reflect(void)
{
    cube_t src = { 0, 0 };
    cube_t ref = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 3; y++) {
            for (int z = 0; z < 5; z++) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 2; y < 5; y++) {
            for (int z = 0; z < 5; z++) {
                cube_set(&ref, x, y, z, 1);
            }
        }
    }

    cube_reflect_group12(&dst, &src);
    if (!cube_equal(&dst, &ref)) {
        printf("0x%016lx%016lx -> 0x%016lx%016lx\n",
               src.high, src.low, dst.high, dst.low);
        return false;
    }

    return true;
}

static bool
test_group24_transforms(void)
{
    cube_t src = { 0, 0 };
    cube_t dst = { 0, 0 };

    for (int x = 0; x < CUBE_MAXX; x += 4) {
        for (int y = 0; y < CUBE_MAXY; y += 4) {
            for (int z = 0; z < CUBE_MAXZ; z += 4) {
                cube_set(&src, x, y, z, 1);
            }
        }
    }

    for (int t = 0; t < CUBE_GROUP24_TRANSFORMS; t++) {
        cube_transform_group24(&dst, &src, t);
        if (!cube_equal(&dst, &src)) {
            printf("%d: 0x%016lx%016lx -> 0x%016lx%016lx\n", t,
                  src.high, src.low, dst.high, dst.low);
            return false;
        }
    }

    return true;
}

static bool
test_cube_filled(void)
{
    cube_t filled = { 0, 0 };
    cube_t notfilled = { 0, 0 };

    for (int x = 0; x < CUBE_MAXX; x++) {
        for (int y = 0; y < CUBE_MAXY; y++) {
            for (int z = 0; z < CUBE_MAXZ; z++) {
                cube_set(&filled, x, y, z, 1);
                cube_set(&notfilled, x, y, z, 1);
            }
        }
    }
    cube_set(&notfilled, 0, 0, 0, 0);

    if (!cube_filled(&filled)) {
        printf("0x%016lx%016lx\n", filled.high, filled.low);
        return false;
    }
    if (cube_filled(&notfilled)) {
        printf("0x%016lx%016lx\n", notfilled.high, notfilled.low);
        return false;
    }

    return true;
}

static bool
test_cube_subtract(void)
{
    cube_t left = CUBE_EMPTY;
    cube_t right = CUBE_EMPTY;
    cube_t expected = CUBE_EMPTY;
    cube_t actual = CUBE_EMPTY;

    cube_set(&left, 1, 1, 1, 1);
    cube_set(&left, 1, 2, 1, 1);
    cube_set(&left, 1, 2, 3, 1);
    cube_set(&left, 2, 2, 3, 1);

    cube_set(&right, 1, 2, 3, 1);
    cube_set(&right, 2, 2, 3, 1);

    cube_set(&expected, 1, 1, 1, 1);
    cube_set(&expected, 1, 2, 1, 1);

    cube_subtract(&actual, &left, &right);
    if (!cube_equal(&actual, &expected)) {
        printf("0x%016lx%016lx\n", actual.high, actual.low);
        return false;
    }

    return true;
}


typedef struct test_t {
    char *name;
    bool (*func)(void);
} test_t;

#define TEST(name) { #name, &name }
    

static test_t tests[] = {
    TEST(test_group3_transforms),
    TEST(test_group3_reflect),
    TEST(test_group6_transforms),
    TEST(test_group6_reflect),
    TEST(test_group12_transforms),
    TEST(test_group12_reflect),
    TEST(test_group24_transforms),
    TEST(test_cube_filled),
    TEST(test_cube_subtract),
    { NULL, NULL },
};
    
int
main(unused int argc, unused char *argv[])
{
    test_t *test = tests;
    bool status = true;

    while (test->name != NULL) {
        bool status1 = test->func();
        printf("%s: %s\n", test->name, status1 ? "passed" : "FAILED");
        status = status1 && status;
        test++;
    }
    
    return (status ? 0 : 1);
}
