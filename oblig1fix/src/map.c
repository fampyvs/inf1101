#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "map.h"
#include "defs.h"
#include "avl.h"




// made by me

map_t *map_create(cmp_fn cmpfn) {
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        return NULL;
    }

    map->length = 0;
    map->root = NULL;
    map->cmpfn = cmpfn; // used to compare keys, e.g. strcmp

    return map;
}

// Made by me

void map_destroy(map_t *map, free_fn val_freefn) {
    if (map == NULL) {
        return;
    }

    // recursively destroy the tree, free the map
    destroy_node(map->root, val_freefn);
    free(map);
}

// Made by me

size_t map_length(map_t *map) {
    if (map == NULL) {
        return 0;
    }
    return map->length;

}

// Made by me - Its included in chat_log1 but i never got any ai assistance on it

void *map_insert(map_t *map, void *key, size_t key_size, void *value) {
    if (map == NULL || value == NULL || key == NULL) {
        return NULL;
    }

    void *old_value = NULL; // used to store and return the value of it exists
    // starting from root, insert the new node
    map->root = node_insert(map->root, key, key_size, value, map->cmpfn, &old_value);

    // increment the length if the key was inserted
    if (old_value == NULL) {
        map->length++;
    }

    // return the old value or NULL if it did not exist
    return old_value;
}



// made by me

void *map_remove(map_t *map, void *key) {
    // check if the map and key is NULL
    if (map == NULL || key == NULL) {
        return NULL;
    }

    
    void *old_value = NULL; // used to store and return the old value of it exists
    // starting from root, remove the node
    map->root = node_remove(map->root, key, map->cmpfn, &old_value);

    // decrement the length if the key was removed
    if (old_value != NULL) {
        map->length--;
    }
    
    // return the old value or NULL if it did not exist
    return old_value;
}


// made by me - Got a small, insignificant helping hand from ai - Chat_log1

void *map_get(map_t *map, void *key) {
    // Check if the map and key is NULL
    if (map == NULL || key == NULL) {
        return NULL;
    }

    // start at root
    node_t *currentNode = map->root;

    while (currentNode != NULL) {
        int cmp = map->cmpfn(key, currentNode->key); // compare the keys with the current nodes key

        // if the key is less than the current node, go left
        if (cmp < 0) {
            currentNode = currentNode->left;

        // if the key is greater than the current node, go right
        } else if (cmp > 0) {
            currentNode = currentNode->right;

        // else the key is equal to the current node, return the value
        } else {
            return currentNode->value;
        }
    }

    // key not found
    return NULL;
}
