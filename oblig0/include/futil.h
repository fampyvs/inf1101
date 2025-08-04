/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#ifndef FUTIL_H
#define FUTIL_H

#include "common.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * @param fpathlike: filepath-like string. Does not need to exist on disk.
 * @returns a pointer to final component of the path (e.g. "mypath/myfile.png" -> "myfile.png")
 */
char *basename(const char *fpathlike);

/**
 * @param c: character/int
 * @returns 1 if the character is a newline, otherwise 0
 */
int isnewline(int c);

/**
 * @brief Powerful and versatile utility to read, filter, delimit and/or convert file content.
 *
 * @param f: pointer to open file
 * @param list: pointer to list. Any tokens will be added last to the list, in the order they appear
 * in the file.
 * @param strlen_min: ommit tokens of a length lower than this
 * @param csplitfn: pointer to function. Called on each character. Split into token when returned
 * value is non-zero. The split character(s) is not included in the token.
 * @param cfilterfn: nullable. If present, the function is called on each character. If the returned
 * value is zero, that character is ommitted from the token.
 * @param ctransformfn: nullable. If present, the function is called on each character. The returned
 * character is added to the token in place of the original character. Applied after filter, if
 * present.
 *
 * @returns 0 on success, otherwise a negative error code
 *
 * @note in the event of an error, the given list is not modified. 'ctype.h' contains a lot of
 * functions that may be passed to this function as arguments, or a custom function may be created.
 *
 * @example
 * - `ftokenize(<file>, <list>, 1, isnewline, isprint, NULL);` -> list of non-empty lines, with
 * non-space escape characters removed
 * - `ftokenize(<file>, <list>, 2, isspace, isalnum, to_lower);` -> list of all words with len >= 2,
 * with special characters removed, converted to lowercase
 *
 * @see
 * https://www.ibm.com/docs/en/aix/7.3?topic=libraries-list-character-manipulation-subroutines
 */
int ftokenize(FILE *f,
              list_t *list,
              size_t strlen_min,
              int (*csplitfn)(int),
              int (*cfilterfn)(int),
              int (*ctransformfn)(int));


#endif /* FUTIL_H */