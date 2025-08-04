/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>

/* attribute intentional switch case fallthrough (silence warning) */
#define ATTR_FALLTHROUGH __attribute__((fallthrough))

/* attribute intentionally unused function (silence warning) */
#define ATTR_UNUSED __attribute__((unused))


/**
 * @brief Type of comparison function
 *
 * @returns
 * - 0 on equality
 *
 * - >0 if a > b
 *
 * - <0 if a < b
 *
 * @note When passing references to comparison functions as parameters
 * typecast to cmp_fn if said functions' parameters are non-void pointers.
 */
typedef int (*cmp_fn)(const void *, const void *);

/**
 * @brief Type of free (resource deallocation) function
 * @note may be the actual `free` function, or another user-defined function
 */
typedef void (*free_fn)(void *);

/**
 * @brief Compare two integers.
 * @param a,b: pointer to integer
 * @returns see `cmp_fn`
 */
int intcmp(const int *a, const int *b);

/**
 * @brief Compare two characters. If strings are passed, only the first character is compared
 * @param a,b: pointer to character
 * @returns see `cmp_fn`
 */
int charcmp(const char *a, const char *b);


#endif /* COMMON_H */