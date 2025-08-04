/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#ifdef LOG_LEVEL
#  undef LOG_LEVEL
#endif /* LOG_LEVEL */

#define LOG_LEVEL LOG_LEVEL_INFO

#include "printing.h"
#include "testutil.h"
#include "tests.h"
#include "defs.h"
#include "common.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

/* struct to bundle some global variable */
typedef struct test_vars {
    char *values[N_TEST_ENTRIES];
    char *values_cc[N_TEST_ENTRIES];
    list_t *list;
    size_t n_tests;
    size_t testno;
    dealloc_stats_t dealloc_stats;
} test_vars_t;

/* file scope wrapper for Test Variables */
static test_vars_t TV = {};

/* wrap free to track calls by reference done by list_destroy */
static void track_free_rd(void *val) {
    track_free(val, &TV.dealloc_stats);
}

static int init_TV() {
    /* set up the test-values */
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        int val = RANDOMIZE_VALUES ? rand() : (int) i;
        char buf[0x7f];
        snprintf(buf, 0x7f, "%d", val);

        TV.values[i] = strdup(buf);
        TV.values_cc[i] = strdup(buf);
    }

    pr_info("- Created %d test-entries -\n", N_TEST_ENTRIES);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        pr_info("\"%s\"%s", TV.values[i], (i == N_TEST_ENTRIES - 1) ? "]\n" : ", ");

        if (!PRINT_ALL_TEST_VALUES && N_TEST_ENTRIES > 16 && i == 9) {
            pr_info("..., ");
            i = N_TEST_ENTRIES - 3;
        }
    }
    pr_info("\n");

    dealloc_stats_init(&TV.dealloc_stats, (void **) TV.values, N_TEST_ENTRIES);

    return 0;
}

static int test_list_create() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[cmpfn=`strcmp`]", list_create);
    TV.list = list_create((cmp_fn) strcmp);
    TEST_ASSERT_OK(TV.list != NULL, "expected pointer to list");

    return 0;
}

static int test_list_add_pop_last() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "", list_addlast, list_contains);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        char *item = TV.values[i];
        int rc = list_addlast(TV.list, item);
        TEST_ASSERT(rc == 0,
                    "list_addlast failed. "
                    "value no. %zu - item: \"%s\"",
                    i, item);
        TEST_ASSERT(list_contains(TV.list, item),
                    "list_contains failed to find the item that was just added. "
                    "value no. %zu - item: \"%s\"",
                    i, item);
        size_t exp_len = i + 1;
        TEST_ASSERT(list_length(TV.list) == exp_len, "expected list_length of %zu, found %zu", exp_len,
                    list_length(TV.list));
    }
    TEST_ASSERT(list_length(TV.list) == N_TEST_ENTRIES, "expected list_length of %d, found %zu",
                N_TEST_ENTRIES, list_length(TV.list));
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[All added values]", list_poplast, list_contains);
    for (size_t i = N_TEST_ENTRIES; i > 0; i--) {
        char *fnd = list_poplast(TV.list);
        char *exp = TV.values[i - 1];
        TEST_ASSERT(fnd == exp,
                    "list_poplast returned the wrong item. "
                    "Expected \"%s\", Found \"%s\".",
                    exp, (fnd ? fnd : "NULL"));

        size_t exp_len = i - 1;
        TEST_ASSERT(list_length(TV.list) == exp_len, "expected list_length of %zu, found %zu", exp_len,
                    list_length(TV.list));
    }
    TEST_ASSERT(list_length(TV.list) == 0, "expected list_length of 0, found %zu", list_length(TV.list));
    TEST_OK();

    return 0;
}

static int test_list_add_pop_first() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "", list_addfirst, list_contains);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        int rc = list_addfirst(TV.list, TV.values[i]);
        TEST_ASSERT(rc == 0,
                    "list_addfirst failed. "
                    "value no. %zu - item: \"%s\"",
                    i, TV.values[i]);
        TEST_ASSERT(list_contains(TV.list, TV.values[i]),
                    "list_contains failed to find the item that was just added. "
                    "value no. %zu - item: \"%s\"",
                    i, TV.values[i]);
        size_t exp_len = i + 1;
        TEST_ASSERT(list_length(TV.list) == exp_len, "expected list_length of %zu, found %zu", exp_len,
                    list_length(TV.list));
    }
    TEST_ASSERT(list_length(TV.list) == N_TEST_ENTRIES, "expected list_length of %d, found %zu",
                N_TEST_ENTRIES, list_length(TV.list));
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[All added values]", list_popfirst, list_length);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        size_t val_i = N_TEST_ENTRIES - 1 - i;
        char *fnd = list_popfirst(TV.list);
        char *exp = TV.values[val_i];
        TEST_ASSERT(fnd == exp,
                    "list_popfirst returned the wrong item. "
                    "Expected \"%s\", Found \"%s\".",
                    exp, (fnd ? fnd : "NULL"));

        TEST_ASSERT(list_length(TV.list) == val_i, "expected list_length of %zu, found %zu", val_i,
                    list_length(TV.list));
    }
    TEST_ASSERT(list_length(TV.list) == 0, "expected list_length of 0, found %zu", list_length(TV.list));
    TEST_OK();

    return 0;
}

static int test_list_cmpfn_and_sort() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[correct cmpfn use]", list_contains);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        char *item = TV.values[i];
        char *item_cc = TV.values_cc[i];

        if (i % 2 == 0) {
            TEST_ASSERT(list_addfirst(TV.list, item) == 0,
                        "list_addfirst failed. "
                        "value no. %zu - item: \"%s\"",
                        i, item);
        } else {
            TEST_ASSERT(list_addlast(TV.list, item) == 0,
                        "list_addlast failed. "
                        "value no. %zu - item: \"%s\"",
                        i, item);
        }

        TEST_ASSERT(list_contains(TV.list, item),
                    "list_contains failed to find the item that was just added. "
                    "value no. %zu - item: \"%s\"",
                    i, item);

        /* check correct utilization of cmpfn */
        TEST_ASSERT(list_contains(TV.list, item_cc),
                    "list_contains failed to find the item that was just added. "
                    "possible cause: failure to utilize cmpfn correctly.  "
                    "value no. %zu - item: \"%s\"",
                    i, item_cc);

        /* modify the string, and compare again. it should be found, as list should just hold a reference. */
        char *c = item;
        size_t len = 0;
        while (*c) {
            *c = 'X';
            len++;
            c++;
        }

        /* check correct utilization of cmpfn */
        TEST_ASSERT(list_contains(TV.list, item),
                    "list_contains failed to find the item that was just added. "
                    "possible cause: failure to utilize cmpfn correctly.  "
                    "value no. %zu - item: \"%s\"",
                    i, item);

        /* restore the modified string */
        memcpy(item, item_cc, len);

        size_t exp_len = i + 1;
        TEST_ASSERT(list_length(TV.list) == exp_len, "expected list_length of %zu, found %zu", exp_len,
                    list_length(TV.list));
    }
    TEST_ASSERT(list_length(TV.list) == N_TEST_ENTRIES, "expected list_length of %d, found %zu",
                N_TEST_ENTRIES, list_length(TV.list));
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[pointer/cmpfn usage]", list_contains);
    /* randomize all test-values, and ensure they are still found */
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        char *c = TV.values[i];
        /* replace each character in val with a random lowercase character */
        while (*c) {
            *c = 'a' + rand() % ('z' - 'a');
            c++;
        }
        /* check correct utilization of cmpfn */
        TEST_ASSERT(list_contains(TV.list, TV.values[i]),
                    "list_contains failed to find the item after it was modified. "
                    "possible cause: storing item, rather than a pointer to it  "
                    "value no. %zu - item: \"%s\"",
                    i, TV.values[i]);
    }
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[sort order]", list_sort);

    list_sort(TV.list);
    char *prev = NULL;

    /* randomize all string values */
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        char *item = list_popfirst(TV.list);
        TEST_ASSERT(item != NULL,
                    "list_popfirst failed to return an item. "
                    "value no. %zu - item: \"%s\"",
                    i, item);

        TEST_ASSERT((!prev || (strcmp(prev, item) <= 0)),
                    "based on the returned item from list_popfirst, the list was not sorted correctly. "
                    "value no. %zu - item: \"%s\"",
                    i, item);

        prev = item;

        size_t exp_len = N_TEST_ENTRIES - 1 - i;
        TEST_ASSERT(list_length(TV.list) == exp_len, "expected list_length of %zu, found %zu", exp_len,
                    list_length(TV.list));
    }
    TEST_ASSERT(list_length(TV.list) == 0, "expected list_length of %d, found %zu", 0, list_length(TV.list));
    TEST_OK();

    return 0;
}

static int test_list_destroy() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[val_freefn=`free`]", list_destroy);

    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        int rc = list_addfirst(TV.list, TV.values[i]);
        TEST_ASSERT(rc == 0,
                    "list_addfirst failed. "
                    "value no. %zu - item: \"%s\"",
                    i, TV.values[i]);
    }
    TEST_ASSERT(list_length(TV.list) == N_TEST_ENTRIES, "expected list_length of %d, found %zu",
                N_TEST_ENTRIES, list_length(TV.list));

    list_destroy(TV.list, track_free_rd);
    TEST_ASSERT(TV.dealloc_stats.n_bad_free_callbacks == 0,
                "list_destroy should ONLY call the referenced function on each list value. "
                "Found %zu calls on non-value pointers.",
                TV.dealloc_stats.n_bad_free_callbacks);
    TEST_ASSERT(TV.dealloc_stats.n_freed == N_TEST_ENTRIES,
                "list_destroy should call the referenced function on each value. "
                "Found n_freed=%zu, expected %d.",
                TV.dealloc_stats.n_freed, N_TEST_ENTRIES);
    TEST_OK();

    /* clean up copied values */
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        free(TV.values_cc[i]);
    }

    return 0;
}

int test_interface_list() {
    assert(N_TEST_ENTRIES >= 1);
    assert(N_TEST_ENTRIES < INT32_MAX);

    pr_info("--- Beginning list test, N_TEST_ENTRIES = %d ---\n", N_TEST_ENTRIES);

    if (init_TV()) {
        pr_error(
            "Failed to allocate memory for the test values. If N_TEST_ENTRIES is very large, lower it.\n");
        return -1;
    }

    const test_group_t test_groups[] = {
        {        test_list_create, 1},
        {  test_list_add_pop_last, 2},
        { test_list_add_pop_first, 2},
        {test_list_cmpfn_and_sort, 3},
        {       test_list_destroy, 1},
    };

    size_t n_test_groups = (sizeof(test_groups) / sizeof(test_group_t));

    for (size_t i = 0; i < n_test_groups; i++) {
        TV.n_tests += test_groups[i].n_subtests;
    }

    for (size_t i = 0; i < n_test_groups; i++) {
        if (test_groups[i].test_group_fn()) {
            return -1;
        }
    }

    pr_info("--- %s: passed ---\n\n", __func__);

    return 0;
}