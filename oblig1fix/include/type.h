#ifndef TYPES_H
#define TYPES_H

#include "defs.h"

// Made by me, inspired by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
typedef struct node_t {
    void *key;
    void *value;
    struct node_t *left;
    struct node_t *right;
    int height;
} node_t;

typedef struct map_t {
    node_t *root;
    size_t length;
    cmp_fn cmpfn;
} map_t;

#endif  // TYPES_H