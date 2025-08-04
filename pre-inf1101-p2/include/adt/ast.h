#ifndef AST_H
#define AST_H

#include <stddef.h>

#include "list.h"
#include "set.h"

// forward declaration - from ai
struct index;
typedef struct index index_t;


// Inspitation from https://keleshev.com/abstract-syntax-tree-an-example-in-c/ and
// https://www.reddit.com/r/C_Programming/comments/lzq2t2/how_to_make_an_ast_in_c/
// This reddit post also assisted in other parts of making the ast
// typedef enum for the type of AST node
typedef struct AST AST;
struct AST {
    enum {
        AST_TERM,
        AST_AND,
        AST_OR,
        AST_ANDNOT
    } type; // type of node
    union {
        char *term; // a search keyword
        struct {
            AST *left;  // left child
            AST *right; // right child
        } children;
    } data; // data of node
};




/* Creation and Deletion */

/* Create a new ast for a term */
AST *ast_create_term(char *term);

/* Create a new ast for an operator */
AST *ast_create_operator(int type, AST *left, AST *right);

/* Frees the memory of the ast nodes and it children */
void ast_destroy(AST *node);



/* Parsing */

/* Parse a list of tokens to the ast */
AST *parse_expression(list_iter_t *token_iter, char *errmsg);



/* Utility */

/* Traverses and evaluates the ast returning the result as a set of documents */
set_t *ast_result(AST *node, index_t *index, char *errmsg);




#endif