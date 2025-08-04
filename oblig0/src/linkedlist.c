/**
 * @authors
 * Steffen Viken Valvaag
 * Odin Bjerke <odin.bjerke@uit.no>
 *  Håkon Bjørkmo
 */

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


// initializes a new linked list
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

// frees the memory allocated for the linked list and its items
void list_destroy(list_t *list, free_fn item_free) {
    if (list == NULL) {
        return;
    }

    lnode_t *node = list->head;
    while (node != NULL) {
        lnode_t *next = node->next;

        if (item_free != NULL) {
            item_free(node->item);
        }

        free(node);
        node = next;
    }

    free(list);
}

// returns the number of items in the linked list
size_t list_length(list_t *list) {
    return list->length;
}

// adds an item to the start of the linked list
int list_addfirst(list_t *list, void *item) {
    lnode_t *node = malloc(sizeof(lnode_t));
    if (node == NULL) {
        return -1;
    }

    node->item = item;
    node->next = list->head;
    node->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = node;
    }
    if (list->tail == NULL) {
        list->tail = node;
    }

    list->head = node;
    list->length++;

    return 0;
}

// adds an item to the end of the linked list
int list_addlast(list_t *list, void *item) {
    lnode_t *node = malloc(sizeof(lnode_t));
    if (node == NULL) {
        return -1;
    }

    node->item = item;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail != NULL) {
        list->tail->next = node;
    }
    if (list->head == NULL) {
        list->head = node;
    }
    
    list->tail = node;
    list->length++;

    return 0;
}

// removes the first item from the linked list
void *list_popfirst(list_t *list) {
    if (list->head == NULL) {
        return NULL;
    }

    lnode_t *node = list->head;
    void *item = node->item;
    list->head = node->next;

    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    free(node);
    list->length--;

    return item;
}

// removes the last item from the linked list
void *list_poplast(list_t *list) {
    if (list->tail == NULL) {
        return NULL;
    }

    lnode_t *node = list->tail;
    void *item = node->item;
    list->tail = node->prev;

    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    free(node);
    list->length--;

    return item;
}

// checks if the list contains the specified item
int list_contains(list_t *list, void *item) {
    if (list == NULL) {
        return 0;
    }

    lnode_t *node = list->head;
    while (node != NULL) {
        if (node->item == item) {
            return 1;
        }
        node = node->next;
    }

    return 0;
}


/* ---- list iterator ---- */

// creates a new iterator for the linked list
list_iter_t *list_createiter(list_t *list) {
    list_iter_t *iter = malloc(sizeof(list_iter_t));
    if (iter == NULL) {
        return NULL;
    }

    iter->list = list;
    iter->node = list->head;

    return iter;
}

// frees the memory allocated for the iterator
void list_destroyiter(list_iter_t *iter) {
    free(iter);
}

// checks if the iterator has a next item
int list_hasnext(list_iter_t *iter) {
    return iter->node != NULL;
}

// returns the next item in the iterator
void *list_next(list_iter_t *iter) {
    if (iter->node == NULL) {
        return NULL;
    }

    void *item = iter->node->item;
    iter->node = iter->node->next;

    return item;
}

// resets the iterator to the start of the linked list
void list_resetiter(list_iter_t *iter) {
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
