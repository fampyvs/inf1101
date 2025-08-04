/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#ifndef TESTUTIL_H
#define TESTUTIL_H

#include "defs.h"
#include "printing.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct dealloc_stats {
    void **to_free;
    size_t n_to_free;
    size_t n_freed;
    size_t n_bad_free_callbacks;
} dealloc_stats_t;

typedef struct test_group {
    int (*test_group_fn)(void);
    size_t n_subtests;
} test_group_t;


/* wrap free to track calls by reference */
void track_free(void *bufp, dealloc_stats_t *stats);

/* initialize and/or reset a free_stats_t struct */
void dealloc_stats_init(dealloc_stats_t *stats, void **to_free, size_t n_to_free);


/* half-assed macros to make testing a bit easier */

#define TEST_BEGIN(testno, ntotal, info, ...)                                               \
    do {                                                                                    \
        fprintf(stderr, ANSI_COLOR_WHI "TEST %2zu/%zu: " ANSI_COLOR_RESET, testno, ntotal); \
        fprintf(stderr, ANSI_COLOR_BLU " %-30s" ANSI_COLOR_RESET, #__VA_ARGS__);            \
        fprintf(stderr, ANSI_COLOR_YEL " %-30s " ANSI_COLOR_RESET, info);                   \
    } while (0)

#define TEST_OK()                                                        \
    do {                                                                 \
        fprintf(stderr, "> " ANSI_COLOR_GRE "OK" ANSI_COLOR_RESET "\n"); \
    } while (0)

#define TEST_ASSERT(assertion, fmt, ...)                                                                  \
    do {                                                                                                  \
        if (assertion) {                                                                                  \
            (void) 0;                                                                                     \
        } else {                                                                                          \
            fprintf(stderr, "> " ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "\n");                            \
            fprintf(stderr, "> " ANSI_COLOR_RED "ASSERTION FAILED" ANSI_COLOR_RESET ": `(%s)`\n",         \
                    #assertion);                                                                          \
            fprintf(stderr, "> " ANSI_COLOR_RED "SOURCE" ANSI_COLOR_RESET ": " META_FILE_LINE_FMT "\n",   \
                    META_FILE_LINE_ARGS);                                                                 \
            fprintf(stderr, "> " ANSI_COLOR_RED "DETAILS" ANSI_COLOR_RESET ": " fmt "\n", ##__VA_ARGS__); \
            return -1;                                                                                    \
        }                                                                                                 \
    } while (0)

#define TEST_ASSERT_OK(assertion, errfmt, ...)         \
    do {                                               \
        TEST_ASSERT(assertion, errfmt, ##__VA_ARGS__); \
        TEST_OK();                                     \
    } while (0)


#endif /* TESTUTIL_H */