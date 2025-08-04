#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "type.h"
#include "map.h"
#include "defs.h"
#include "avl.h"



node_t *create_node(void *key, size_t key_size, void *value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL) {
        return NULL;
    }

    // allocate memory for the key by its key_size, if it fails, free the node
    new_node->key = malloc(key_size);
    if (new_node->key == NULL) {
        free(new_node);
        return NULL;
    }
    // then copy the key, set the value and height
    memcpy(new_node->key, key, key_size);
    new_node->value = value;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->height = 1;

    return new_node;
}


// MAde by me - I dont know if this is correct?
// I might just be overthinking it, misunderstanding something?

void destroy_node(node_t *node, free_fn val_freefn) {
    if (node == NULL) {
        return;
    }

    // recursively destroy the left and right nodes
    destroy_node(node->left, val_freefn);
    destroy_node(node->right, val_freefn);

    // free the value
    if (val_freefn != NULL) {  
        val_freefn(node->value);
    }

    // free the key and node
    free(node->key);
    free(node);
}


node_t *node_insert(node_t *node, void *key, size_t key_size, void *value, cmp_fn cmpfn, void **old_value) {
    if (node == NULL) {
        // create a new node and balance the tree 
        node = create_node(key, key_size, value);
    } else if (cmpfn(key, node->key) < 0) {
        // recursively insert into the left subtree and balance the tree
        node->left = node_insert(node->left, key, key_size, value, cmpfn, old_value);
        node = balance_tree(node, key, cmpfn);
    } else if (cmpfn(key, node->key) > 0) {
        // recursively insert into the right subtree and balance the tree
        node->right = node_insert(node->right, key, key_size, value, cmpfn, old_value);
        node = balance_tree(node, key, cmpfn);
    } else {
        // already exists, update values and balance the tree
        *old_value = node->value;
        node->value = value;
        node = balance_tree(node, key, cmpfn);
    }

    // return the new node
    return node;
}




node_t *balance_tree(node_t *node, void *key, cmp_fn cmpfn) {
    // Update the height of the node, check if the node is unbalanced
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = get_balance(node);

    // Rebalance the left left case
    if (balance > 1 && cmpfn(key, node->left->key) < 0) {
        return right_rotate(node);
    }

    // Rebalance the right right case
    if (balance < -1 && cmpfn(key, node->right->key) > 0) {
        return left_rotate(node);
    }
    
    // Rebalance the left right case
    if (balance > 1 && cmpfn(key, node->left->key) > 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    // Rebalance the right left case
    if (balance < -1 && cmpfn(key, node->right->key) < 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    
    return node;
}



node_t *node_remove(node_t *node, void *key, cmp_fn cmpfn, void **old_value) {
    if (node == NULL) {
        return NULL;
    }

    // compare the key with the current node
    int comp = cmpfn(key, node->key);

    // if the key is less than the current node, recursively remove from the left subtree
    if (comp < 0) {
        node->left = node_remove(node->left, key, cmpfn, old_value);

    // if the key is greater than the current node, recursively remove from the right subtree
    } else if (comp > 0) {
        node->right = node_remove(node->right, key, cmpfn, old_value);

    } else { // if the key is equal to the current node, store the old value associated with the key
        *old_value = node->value;

        // no children in the tree
        if (node->left == NULL && node->right == NULL) {
            // free the key and node, return NULL
            free(node->key);
            free(node);
            return NULL;
        }

        // one child in the tree
        if (node->left == NULL) {
            // store right child and free current node
            node_t *right_c = node->right;
            free(node->key);
            free(node);
            // return right child
            return right_c;
        }

        // same as above but for left child
        if (node->right == NULL) {
            node_t *left_c = node->left;
            free(node->key);
            free(node);
            return left_c;
        }

        // Node has two children
        node_t *successor = node->right;
        node_t *parent = node;

        // find the successor
        while (successor->left != NULL) {
            parent = successor;
            successor = successor->left;
        }

        // replace the current nodes key and value with the successor node
        char *old_key = node->key;
        node->key = malloc(strlen(successor->key) + 1);
        strcpy(node->key, successor->key);
        node->value = successor->value;

        // update the parents child node with the right child of the successor
        if (parent->left == successor) {
            parent->left = successor->right;
        } else {
            parent->right = successor->right;
        }

        free(successor->key);
        free(successor);
        free(old_key);
    }

    // Made by https://www.geeksforgeeks.org/deletion-in-an-avl-tree/
    // Update the height of the node, check if the node is unbalanced
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = get_balance(node);

    // Rebalance the left left case
    if (balance > 1 && get_balance(node->left) >= 0) {
        return right_rotate(node);
    }

    // Rebalance the left right case
    if (balance > 1 && get_balance(node->left) < 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    // Rebalance the right right case
    if (balance < -1 && get_balance(node->right) <= 0) {
        return left_rotate(node);
    }

    // Rebalance the left right case
    if (balance < -1 && get_balance(node->right) > 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}




int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}




node_t *right_rotate(node_t *y) {
    node_t *x = y->left;
    node_t *T2 = x->right;

    // Rotate
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}




node_t *left_rotate(node_t *x) {
    node_t *y = x->right;
    node_t *T2 = y->left;

    // Rotate
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}



int is_balanced(node_t *node) {
    if (node == NULL) {
        return 1;
    }

    int balance = get_balance(node);
    if (balance > 1 || balance < -1) {
        return 0;
    }

    return is_balanced(node->left) && is_balanced(node->right);
}



int get_balance(node_t *node) {
    if (node != NULL) {
        return height(node->left) - height(node->right);
    } else {
        return 0;
    }
}




int height(node_t *node) {
    if (node != NULL) {
        return node->height;
    } else {
        return 0;
    }
}


