/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#ifndef TESTS_H
#define TESTS_H

#include "defs.h"
#include <stdbool.h>


/* --- Parameters/options --- */

/**
 * how many entries to test with? After passing with a low amount, try increasing drastically, to at least
 * something like 10000.
 */
#define N_TEST_ENTRIES 12345

/**
 * whether or not to print a list of every generated value before starting the test.
 * If false, a few values are still printed.
 */
#define PRINT_ALL_TEST_VALUES false

/**
 * Set to true to randomize values.
 */
#define RANDOMIZE_VALUES false

/* -------------------------- */


/**
 * @brief Run a set of tests on the `map.h` implementation.
 * @returns 0 on success, otherwise a negative error code
 */
int test_interface_map();

/**
 * @brief Run a set of tests on the `list.h` implementation.
 * @returns 0 on success, otherwise a negative error code
 */
int test_interface_list();

#endif /* TESTS_H */