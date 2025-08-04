/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#include "common.h"
#include "printing.h"

#include <string.h>
#include <stdio.h>


int intcmp(const int *a, const int *b) {
    return *a - *b;
}

int charcmp(const char *a, const char *b) {
    return (int) (*a - *b);
}

char *basename(const char *fpathlike) {
    char *s = strrchr(fpathlike, '/');

    if (s && ++s) {
        return s;
    }
    return (char *) fpathlike;
}
