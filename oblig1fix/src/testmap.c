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
#include "map.h"

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
    char *keys[N_TEST_ENTRIES];
    char *keys_cc[N_TEST_ENTRIES];
    int *values[N_TEST_ENTRIES];
    int *values_cc[N_TEST_ENTRIES];
    map_t *map;
    size_t n_tests;
    size_t testno;
    dealloc_stats_t dealloc_stats;
} test_vars_t;

/* file scope wrapper for Test Variables */
static test_vars_t TV = {};

/* wrap free to track calls by reference done by map_destroy */
static void track_free_rd(void *val) {
    track_free(val, &TV.dealloc_stats);
}

static int init_TV() {
    int key_max_w = (int) floor(log10(N_TEST_ENTRIES - 1)) + 1;

    /* set up the test-values */
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        char key_buf[20];
        snprintf(key_buf, 20, "k%0*zu", key_max_w, i);

        TV.keys[i] = strdup(key_buf);
        TV.keys_cc[i] = strdup(key_buf);

        int val = RANDOMIZE_VALUES ? rand() : (int) i;
        TV.values[i] = malloc(sizeof(int));
        TV.values_cc[i] = malloc(sizeof(int));

        if (!(TV.keys[i] && TV.keys_cc[i] && TV.values[i] && TV.values_cc[i])) {
            return -1;
        }

        *TV.values[i] = val;
        *TV.values_cc[i] = val;
    }

    pr_info("- Created %d test-entries -\n", N_TEST_ENTRIES);
    pr_info("%-*s   %-*s    %s\n", key_max_w, "no.", key_max_w, "key", "val");
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        pr_info("%-*zu | %s : %d\n", key_max_w, i, TV.keys_cc[i], *TV.values[i]);

        if (!PRINT_ALL_TEST_VALUES && N_TEST_ENTRIES > 16 && i == 9) {
            pr_info("%11s\n", "...");
            i = N_TEST_ENTRIES - 3;
        }
    }
    pr_info("\n");

    dealloc_stats_init(&TV.dealloc_stats, (void **) TV.values, N_TEST_ENTRIES);

    return 0;
}

static int test_map_create() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[cmpfn=`strcmp`, hashfn=`hash_string_fnv1a64`]", map_create);
    TV.map = map_create((cmp_fn) strcmp);
    TEST_ASSERT_OK(TV.map != NULL, "expected pointer to map");

    return 0;
}

static int test_map_insert() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[Unique Keys]", map_insert, map_length);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        TEST_ASSERT(map_length(TV.map) == i, "expected map_length of %zu, found %zu", i,
                    map_length(TV.map));

        size_t key_size = strlen(TV.keys[i]) + 1;
        void *old_val = map_insert(TV.map, TV.keys[i], key_size, TV.values_cc[i]);
        TEST_ASSERT(old_val == NULL,
                    "Expected NULL in return when inserting key not already in the map"
                    " -- Entry #%zu, key=\"%s\"",
                    i, TV.keys[i]);
    }
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[Existing Keys]", map_insert, map_length);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        /* by using keys_cc, we also test that cmpfn is utilized correctly */
        size_t key_size = strlen(TV.keys_cc[i]) + 1;
        void *old_val = map_insert(TV.map, TV.keys_cc[i], key_size, TV.values[i]);

        TEST_ASSERT(old_val == TV.values_cc[i],
                    "Expected old value in return when inserting a new value using existing key"
                    " -- Entry #%zu, key=\"%s\", expected_p=%p, found_p=%p",
                    i, TV.keys_cc[i], (void *) TV.values_cc[i], old_val);
    }
    TEST_ASSERT(map_length(TV.map) == N_TEST_ENTRIES,
                "expected map_length of %d, found %zu. Inserting existing keys should not change length",
                N_TEST_ENTRIES, map_length(TV.map));
    TEST_OK();

    return 0;
}

static int test_map_get() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[Invalid Keys]", map_get);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        /* Invalidate the string. Further cmpfn verification: uncover possible direct pointer comparison */
        TV.keys_cc[i][0] = 0;

        int *val = map_get(TV.map, TV.keys_cc[i]);
        TEST_ASSERT(val == NULL,
                    "Expected NULL in return, as this program has never mapped an item to an empty string. "
                    "The map should copy given keys on insertion, not simply store keys as pointers."
                    " -- Entry #%zu, key=\"%s\", expected_p=%p, found_p=%p",
                    i, TV.keys_cc[i], NULL, val);

        /* won't be using these any more, so free them up. The map should not depend on them. */
        free(TV.keys_cc[i]);

        /* Break values_cc before freeing, to set up the next test. */
        TV.values_cc[i][0] = 0;
        free(TV.values_cc[i]);
    }
    TEST_ASSERT(map_length(TV.map) == N_TEST_ENTRIES,
                "expected map_length of %d, found %zu. map_get should never modify the length of the map.",
                N_TEST_ENTRIES, map_length(TV.map));
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[Valid Keys]", map_get);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        int *val = map_get(TV.map, TV.keys[i]);

        TEST_ASSERT(
            val == TV.values[i],
            "Expected the most recently inserted value in return when calling map_get using a valid key."
            " -- Entry #%zu, key=\"%s\", expected_p=%p, found_p=%p",
            i, TV.keys[i], (void *) TV.values[i], val);
    }
    TEST_ASSERT(map_length(TV.map) == N_TEST_ENTRIES,
                "expected map_length of %d, found %zu. map_get should never modify the length of the map.",
                N_TEST_ENTRIES, map_length(TV.map));
    TEST_OK();

    return 0;
}

static int test_map_remove() {
    TEST_BEGIN(++TV.testno, TV.n_tests, "[Invalid Keys]", map_remove, map_length);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        void *val = map_remove(TV.map, "not a valid key");
        TEST_ASSERT(val == NULL,
                    "Expected NULL in return when attempting to remove with a non-present key from the map."
                    " -- Entry #%zu, key=\"%s\", expected_p=%p, found_p=%p",
                    i, TV.keys[i], NULL, val);
    }
    TEST_ASSERT(map_length(TV.map) == N_TEST_ENTRIES, "expected map_length of %d, found %zu.",
                N_TEST_ENTRIES, map_length(TV.map));
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[Valid Keys]", map_remove, map_length);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        int *val = map_remove(TV.map, TV.keys[i]);

        TEST_ASSERT(val == TV.values[i],
                    "Expected the most recently inserted value for this key."
                    " -- Entry #%zu, key=\"%s\", expected_p=%p, found_p=%p",
                    i, TV.keys[i], (void *) TV.values[i], val);

        size_t expected_length = N_TEST_ENTRIES - 1 - i;
        TEST_ASSERT(
            map_length(TV.map) == expected_length,
            "expected map_length of %zu, found %zu. map_get should never modify the length of the map.",
            expected_length, map_length(TV.map));
    }
    TEST_OK();

    TEST_BEGIN(++TV.testno, TV.n_tests, "[Re-Insert]", map_insert, map_length);
    for (size_t i = 0; i < N_TEST_ENTRIES; i++) {
        TEST_ASSERT(map_length(TV.map) == i, "expected map_length of %zu, found %zu", i,
                    map_length(TV.map));

        size_t key_size = strlen(TV.keys[i]) + 1;
        void *val = map_insert(TV.map, TV.keys[i], key_size, TV.values[i]);
        TEST_ASSERT(val == NULL,
                    "Expected NULL in return when inserting new key that was removed in the previous test."
                    " -- Entry #%zu, key=\"%s\", expected_p=%p, found_p=%p",
                    i, TV.keys[i], NULL, val);
        free(TV.keys[i]);
    }
    TEST_OK();

    return 0;
}

static int test_map_destroy() {
    /* technically track_free, but it's simply a wrapper for free */
    TEST_BEGIN(++TV.testno, TV.n_tests, "[val_freefn=`free`]", map_destroy);
    map_destroy(TV.map, track_free_rd);
    TEST_ASSERT(TV.dealloc_stats.n_bad_free_callbacks == 0,
                "map_destroy should ONLY call the referenced function on each map value. "
                "Found %zu calls on non-value pointers.",
                TV.dealloc_stats.n_bad_free_callbacks);
    TEST_ASSERT(TV.dealloc_stats.n_freed == N_TEST_ENTRIES,
                "map_destroy should call the referenced function on each value. "
                "Found n_freed=%zu, expected %d.",
                TV.dealloc_stats.n_freed, N_TEST_ENTRIES);
    TEST_OK();

    return 0;
}

int test_interface_map() {
    assert(N_TEST_ENTRIES >= 1);
    assert(N_TEST_ENTRIES < INT32_MAX);

    pr_info("--- Beginning map test, N_TEST_ENTRIES = %d ---\n", N_TEST_ENTRIES);

    if (init_TV()) {
        pr_error(
            "Failed to allocate memory for the test values. If N_TEST_ENTRIES is very large, lower it.\n");
        return -1;
    }

    const test_group_t test_groups[] = {
        { test_map_create, 1},
        { test_map_insert, 2},
        {    test_map_get, 2}, // may be commented out to skip this test group
        { test_map_remove, 3}, // may be commented out to skip this test group
        {test_map_destroy, 1},
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
