#ifndef AVL_H
#define AVL_H

#include "defs.h"
#include "map.h"


// Made by me, inspired by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
typedef struct node_t node_t;


// Made by me
/** 
 *  @brief Create a new node with a key and value
 *  @param key: Stored in the node
 *  @param key_size: Size of the key
 *  @param value: Stored in the node
 *  @returns A pointer to the new node, or NULL if allocation failed
 * 
 *  @note The nodes key is copied into a newly allocated buffer, but the value is not copied.
 *        Current implementation does not handle the null-terminator. Here malloc and memcopy is used on the key
*/
node_t *create_node(void *key, size_t key_size, void *value);

// Made by me
/**
 *  @brief Destroys a node and frees its associated memory.
 *  @param node The node to be destroyed.
 *  @param val_freefn A function to free the node's value, if applicable.
 * 
 *  @note The caller is responsible for ensuring that the node is not accessed after it is destroyed.
 */
void destroy_node(node_t *node, free_fn val_freefn);

// Made by me
// inspired by https://www.geeksforgeeks.org/insertion-in-binary-search-tree/
// and https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
// and https://stackoverflow.com/questions/33715301/inserting-nodes-in-a-binary-search-tree-c
/** 
 *  @brief Recursively insert a key value pair into the tree
 *  @param node: Node to insert into
 *  @param key: Key to insert
 *  @param key_size: Size of the key
 *  @param value: Value to insert
 *  @param cmpfn: Function for comparing keys 
 *  @param old_value: Pointer to store the old value
 *  @returns The node after insertion
 * 
 *  @note If the key already exists, its value is updated and the old value is returned through the old_value parameter.
*/
node_t *node_insert(node_t *node, void *key, size_t key_size, void *value, cmp_fn cmpfn, void **old_value);

// For the most part, its made by me. inspired by https://www.geeksforgeeks.org/deletion-in-binary-search-tree/
// and https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
// and https://www.enjoyalgorithms.com/blog/deletion-in-binary-search-tree
/**  
 *  @brief Remove the node with the given key
 *  @param node: Node to be removed
 *  @param key: Key to remove
 *  @param cmpfn: Function for comparing keys
 *  @param old_value: Pointer to store the old value
 *  @returns The node after removal
 * 
 *  @note If the key does not exist, NULL is returned
*/
node_t *node_remove(node_t *node, void *key, cmp_fn cmpfn, void **old_value);



// Everything under was either inspired by or copied from https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
// And https://www.geeksforgeeks.org/deletion-in-an-avl-tree/


// made by https://www.geeksforgeeks.org/insertion-in-an-avl-tree/ 
/**
 *  @brief Helper function for balancing the tree - used in node_insert
 *  @param node: Node to be balanced
 *  @param key: Key to be inserted
 *  @param cmpfn: Function for comparing keys
 *  @returns The balanced node
 * 
 *  @note It uses the AVL tree balancing algorithm to balance the tree.
 *        It also assumes that the input node is not null.
*/
node_t *balance_tree(node_t *node, void *key, cmp_fn cmpfn);

// Made by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
/** 
 *  @brief Performs a right rotation on the binary search tree.
 *  @param y: Node to be rotated
 *  @returns The new root node x after rotation
 * 
 *  @note This function also assumes that the input node y is not null and has a valid left child.
*/
node_t *right_rotate(node_t *y);

// Made by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
/** 
 *  @brief Performs a left rotation on the binary search tree.
 *  @param x: Node to be rotated
 *  @returns The new root node y after rotation
 * 
 *  @note This function also assumes that the input node x is not null and has a valid right child.
*/
node_t *left_rotate(node_t *x);

// Made by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
/** 
 *  @brief Get the height of a node
 *  @param node: Node whose height to be calculated
 *  @returns The height of a node, or 0 if the node is null
 * 
 *  @note 
*/
int height(node_t *node);

// Made by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
/**  
 *  @brief Returns the maximum of two integers.
 *  @param a: First integer
 *  @param b: Second integer
 *  @returns The maximum of a and b
 * 
 *  @note It assumes the inputs are valid integers
*/
int max(int a, int b);

// Made by https://www.geeksforgeeks.org/c-program-to-implement-avl-tree/
/** 
 *  @brief Gets the balance factor of a node
 *  @param node: The node to be calculated
 *  @returns The balance factor of the node,
 *           calculated by subtracting the height of the right subtree 
 *           from the height of the left subtree
 * 
 *  @note A factor of -1, 0 or 1 indicates a balanced node
*/
int get_balance(node_t *node);


// Made by me
/**  
 *  @brief Recursively check if the tree is balanced
 *  @param node: The root node of the tree
 *  @returns If its balanced 1, if not 0
 * 
 *  @note The tree is balanced so long the difference between
 *        the heights of the left and right subtrees is at most 1 
*/
int is_balanced(node_t *node);


#endif /* AVL_H */
