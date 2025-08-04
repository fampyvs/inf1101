/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#include "testutil.h"

#include <stdlib.h>
#include <stdio.h>

void track_free(void *bufp, dealloc_stats_t *stats) {
    if (bufp != NULL) {
        for (size_t i = 0; i < stats->n_to_free; i++) {
            if (stats->to_free[i] == bufp) {
                free(bufp);
                stats->n_freed++;
                stats->to_free[i] = NULL;
                return;
            }
        }
    }

    stats->n_bad_free_callbacks++;
}

void dealloc_stats_init(dealloc_stats_t *stats, void **to_free, size_t n_to_free) {
    stats->n_freed = 0;
    stats->n_bad_free_callbacks = 0;
    stats->n_to_free = n_to_free;
    stats->to_free = to_free;
}