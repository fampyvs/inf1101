/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#ifndef MAP_H
#define MAP_H

#include "defs.h"
#include "avl.h"
#include <stdlib.h>


/**
 * Type of map. `map_t` is an alias for `struct map`
 */
typedef struct map_t map_t;

/**
 * @brief Creates a new, empty map
 * @param cmpfn: function for comparing keys
 * @param hashfn: function for hashing values. Depending on the underlying data structure, 
 * this may not be used. E.g. a Hash Table requires a hash function, a Binary Search Tree does not.
 * @returns A pointer to the newly created map, or NULL on error.
 */
map_t *map_create(cmp_fn cmpfn, hash64_fn hashfn);

/**
 * @brief Destroys the given map. Optional functionality to also destroy values
 * @param map: pointer to a map
 * @param val_freefn: nullable. If present, called on all values
 */
void map_destroy(map_t *map, free_fn val_freefn);

/**
 * @brief Get the number of entries contained in a map, colloquially referred to as its length
 * @param map: pointer to a map
 * @returns The number of entries in `map`
 */
size_t map_length(map_t *map);

/**
 * @brief insert a key-value pair into the map
 * @param map: pointer to map
 * @param key: pointer to key
 * @param key_size: size of `key` in bytes
 * @param value: pointer to non-NULL value
 * @returns NULL if the map did not previously have this key present, otherwise a pointer to the old value at
 * `key`.
 *
 * @note Since NULL is used as a return-value to indicate 'key not found', `value` may not be NULL
 * @note The caller retains ownership of `key`, and may alter/free it without affecting the
 * functionality of the map.
 */
void *map_insert(map_t *map, void *key, size_t key_size, void *value);

/**
 * @brief Attempt to remove an entry from the map
 * @param map: pointer to a map
 * @param key: pointer to key
 * @returns The value at the key; if the key was previously in the map, otherwise NULL.
 */
void *map_remove(map_t *map, void *key);

/**
 * @brief Attempt to get the value at `key`
 * @param map: pointer to a map
 * @param key: pointer to key
 * @returns The value at the key if the key is in the map, otherwise NULL.
 *
 * @note This function also serves as the de-facto utility to check for the presense of a key in the map
 */
void *map_get(map_t *map, void *key);





#endif /* MAP_H */