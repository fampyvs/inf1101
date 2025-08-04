/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#include "futil.h"
#include "common.h"
#include "printing.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <ctype.h>


/* initial buffer size. Must be >0. */
#define INITIAL_BUFSIZE 0x7f


inline int isnewline(int c) {
    return (c == '\n') ? 1 : 0;
}

int ftokenize(FILE *f,
              list_t *list,
              size_t strlen_min,
              int (*csplitfn)(int),
              int (*cfilterfn)(int),
              int (*ctransformfn)(int)) {
    if (!f) {
        pr_error("File pointer is NULL\n");
        return -2;
    }

    int rv = 0;
    size_t list_len_before = list_length(list);
    size_t bufsize = INITIAL_BUFSIZE;

    /* allocate temporary buffer */
    char *buf = malloc(bufsize);
    if (!buf) {
        pr_error("Cannot allocate memory\n");
        return -1;
    }

    char *s = buf;
    size_t len = 0;

    while (!feof(f)) {
        int c = fgetc(f);
        if (c < EOF) {
            pr_error("Failed to read from the file\n");
            rv = c;
            break;
        }

        if (c == EOF || (csplitfn && csplitfn(c))) {
            if (len >= strlen_min) {
                *s = 0;
                char *cpy = strdup(buf);
                if (cpy == NULL) {
                    pr_error("failed to allocate memory\n");
                    rv = -1;
                    break;
                }
                rv = list_addlast(list, cpy);
                if (rv < 0) {
                    pr_error("list_addlast failed\n");
                    free(cpy);
                    break;
                }
            }
            s = buf;
            len = 0;
        } else if (cfilterfn == NULL || cfilterfn(c)) {
            *s = ctransformfn ? ctransformfn(c) : c;
            len++;
            s++;

            if (len == bufsize - 1) {
                /* buffer is full, as one byte is needed for null-terminator */
                bufsize *= 2;
                char *re_buf = realloc(buf, bufsize);
                if (re_buf == NULL) {
                    pr_error("failed to reallocate memory: %s\n", strerror(errno));
                    rv = -1;
                    break;
                }
                buf = re_buf;
            }
        }
    }

    free(buf);

    /* either complete the operation, or revert list state on error */
    if (rv < 0) {
        while (list_length(list) > list_len_before) {
            free(list_poplast(list));
        }
    } else if (list_len_before == list_length(list)) {
        pr_warn("List of words is empty. If the file contains tokens, list_addlast has not worked.\n");
    }

    return rv;
}
