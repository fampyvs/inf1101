// This is from a previous assignment - oblig0
// It's a direct copy but with added comments. Made by me.
// Mergesort from Steffen Viken Valvaag

#include "list.h"
#include "printing.h"

#include <stdlib.h>


typedef struct lnode lnode_t;
struct lnode {
    lnode_t *next;
    lnode_t *prev;
    void *item;
};

struct list {
    lnode_t *head;
    lnode_t *tail;
    size_t length;
    cmp_fn cmpfn;
};

struct list_iter {
    list_t *list;
    lnode_t *node;
};



list_t *list_create(cmp_fn cmpfn) {
list_t *list = malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->cmpfn = cmpfn;

    return list;
}


void list_destroy(list_t *list, free_fn item_free) {
    if (list == NULL) {
        return;
    }

    // start the head node we iterates over and free each node and its item
    lnode_t *node = list->head;
    while (node != NULL) {
        lnode_t *next = node->next;

        if (item_free != NULL) {
            item_free(node->item);
        }

        free(node);
        node = next;
    }

    // Free the list
    free(list);
}

size_t list_length(list_t *list) {
    return list->length;
}


int list_addfirst(list_t *list, void *item) {
    lnode_t *node = malloc(sizeof(lnode_t));
    if (node == NULL) {
        return -1;
    }

    // initialize a new node at the head or start of the list
    node->item = item;
    node->next = list->head;
    node->prev = NULL;

    // update current head node to point to the new node
    if (list->head != NULL) {
        list->head->prev = node;
    }
    // if the list was empty, set the tail to the new node
    if (list->tail == NULL) {
        list->tail = node;
    }

    // update the head to point to the new node
    list->head = node;
    list->length++;

    return 0;
}


int list_addlast(list_t *list, void *item) {
    lnode_t *node = malloc(sizeof(lnode_t));
    if (node == NULL) {
        return -1;
    }

    // initialize a new node at the tail or end of the list
    node->item = item;
    node->next = NULL;
    node->prev = list->tail;

    // update current tail node to point to the new node
    if (list->tail != NULL) {
        list->tail->next = node;
    }
    // if the list was empty, set the head to the new node
    if (list->head == NULL) {
        list->head = node;
    }
    
    // update the tail to point to the new node
    list->tail = node;
    list->length++;

    return 0;
}


void *list_popfirst(list_t *list) {
    if (list->head == NULL) {
        return NULL;
    }

    // remove the current head node, store its item and update the head pointer
    lnode_t *node = list->head;
    void *item = node->item;
    list->head = node->next;

    // update the prev pointer of the new head node
    if (list->head != NULL) {
        list->head->prev = NULL;
    } else { // if the list is now empty
        list->tail = NULL;
    }

    // free the node, decrement the length, and return the item
    free(node);
    list->length--;

    return item;
}


void *list_poplast(list_t *list) {
    if (list->tail == NULL) {
        return NULL;
    }

    // remove the current tail node, store its item and update the tail pointer
    lnode_t *node = list->tail;
    void *item = node->item;
    list->tail = node->prev;

    // update the next pointer of the new tail node
    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else { // if the list is now empty
        list->head = NULL;
    }

    // free the node, decrement the length, and return the item
    free(node);
    list->length--;

    return item;
}


int list_contains(list_t *list, void *item) {
    if (list == NULL) {
        return 0;
    }

    // at the head node we iterate over and check if the item is the same
    lnode_t *node = list->head;
    while (node != NULL) {
        if (list->cmpfn(node->item, item) == 0) { // Made changes here after running the test file
            return 1;
        }
        // go to the next node
        node = node->next;
    }

    return 0;
}


/* ---- list iterator ---- */


list_iter_t *list_createiter(list_t *list) {
    list_iter_t *iter = malloc(sizeof(list_iter_t));
    if (iter == NULL) {
        return NULL;
    }

    // initialize the iterator at the start of the list
    iter->list = list;
    iter->node = list->head;

    return iter;
}


void list_destroyiter(list_iter_t *iter) {
    // free the iterator
    free(iter);
}


int list_hasnext(list_iter_t *iter) {
    // returns true if the iterator has not reached the end of the list
    return iter->node != NULL;
}


void *list_next(list_iter_t *iter) {
    if (iter->node == NULL) {
        return NULL;
    }

    // store the item and go from the current node to the next node
    void *item = iter->node->item;
    iter->node = iter->node->next;

    return item;
}


void list_resetiter(list_iter_t *iter) {
    // reset the iterator to the start of the list
    iter->node = iter->list->head;
}



/* ---- mergesort: Steffen Viken Valvaag ---- */

/*
 * Merges two sorted lists a and b using the given comparison function.
 * Only assigns the next pointers; the prev pointers will have to be
 * fixed by the caller.  Returns the head of the merged list.
 */
static lnode_t *merge(lnode_t *a, lnode_t *b, cmp_fn cmpfn) {
    lnode_t *head, *tail;

    /* Pick the smallest head node */
    if (cmpfn(a->item, b->item) < 0) {
        head = tail = a;
        a = a->next;
    } else {
        head = tail = b;
        b = b->next;
    }

    /* Now repeatedly pick the smallest head node */
    while (a && b) {
        if (cmpfn(a->item, b->item) < 0) {
            tail->next = a;
            tail = a;
            a = a->next;
        } else {
            tail->next = b;
            tail = b;
            b = b->next;
        }
    }

    /* Append the remaining non-empty list (if any) */
    if (a) {
        tail->next = a;
    } else {
        tail->next = b;
    }

    return head;
}

/**
 * Splits the given list in two halves, and returns the head of
 * the second half.
 */
static lnode_t *splitlist(lnode_t *head) {
    /* Move two pointers, a 'slow' one and a 'fast' one which moves
     * twice as fast.  When the fast one reaches the end of the list,
     * the slow one will be at the middle.
     */
    lnode_t *slow = head;
    lnode_t *fast = head->next;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    /* Now 'cut' the list and return the second half */
    lnode_t *half = slow->next;
    slow->next = NULL;

    return half;
}

/**
 * Recursive merge sort.  This function is named mergesort_ to avoid
 * collision with the mergesort function that is defined by the standard
 * library on some platforms.
 */
static lnode_t *mergesort_(lnode_t *head, cmp_fn cmpfn) {
    if (head->next == NULL) {
        return head;
    }

    lnode_t *half = splitlist(head);
    head = mergesort_(head, cmpfn);
    half = mergesort_(half, cmpfn);

    return merge(head, half, cmpfn);
}

void list_sort(list_t *list) {
    /* Recursively sort the list */
    list->head = mergesort_(list->head, list->cmpfn);

    // /* Fix the tail and prev links */
    // lnode_t *prev = NULL;
    // for (lnode_t *n = list->head; n != NULL; n = n->next) {
    //     n->prev = prev;
    //     prev = n;
    // }
    // list->tail = prev;
}