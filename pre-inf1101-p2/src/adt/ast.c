
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#include "defs.h"
#include "list.h"
#include "map.h"
#include "set.h"
#include "printing.h"
#include "common.h"
#include "index.h"
#include "ast.h"


/* forward declaration */
static AST *parse_recursive_expression(list_iter_t *token_iter, char *errmsg);
static AST *parse_andnot(list_iter_t *tokens, char *errmsg);
static AST *parse_or(list_iter_t *tokens, char *errmsg);
static AST *parse_and(list_iter_t *tokens, char *errmsg);
static AST *parse_term(list_iter_t *tokens, char *errmsg);



/* Creation and Deletion */

/* Create a new ast for a term */
AST *ast_create_term(char *term) {
    AST *ast_node = malloc(sizeof(AST));
    if (ast_node == NULL) {
        pr_error("Failed to allocate memory for AST node\n");
        return NULL;
    }

    /* initialize the type, and dup the term to the term field in data*/
    ast_node->type = AST_TERM;
    ast_node->data.term = strdup(term);
    if (ast_node->data.term == NULL) {
        pr_error("Failed to duplicate term string\n");
        free(ast_node);
        return NULL;
    }

    return ast_node;
}

/* Create a new ast for an operator */
AST *ast_create_operator(int type, AST *left, AST *right) {
    AST *ast_node = malloc(sizeof(AST));
    if (ast_node == NULL) {
        pr_error("Failed to allocate memory for AST node\n");
        return NULL;
    }

    /* initialize the node to its respective field in the struct */
    ast_node->type = type;
    ast_node->data.children.left = left;
    ast_node->data.children.right = right;

    return ast_node;
}

/* Frees the memory of the ast nodes and it children */
void ast_destroy(AST *node) {
    if (node == NULL) {
        pr_error("AST node is NULL\n");
        return;
    }

    /* recurcively destroy the ast 
        check the type of node to be freed */
    if (node->type == AST_AND || node->type == AST_OR || node->type == AST_ANDNOT) {
        ast_destroy(node->data.children.left);
        ast_destroy(node->data.children.right);
    }
    else if (node->type == AST_TERM) {
        free(node->data.term);
    }

    free(node);

}



/* Parsing */

/* Parse a list of tokens to the ast
    this is the top level function that calls the recursive part
    helping hand from ai - see chatlog_1 */
AST *parse_expression(list_iter_t *token_iter, char *errmsg) {
    /* calls the recursive part */
    AST *ast = parse_recursive_expression(token_iter, errmsg);

    /* check for failiure */
    if (ast == NULL) {
        return NULL;
    }

    /* performs the leftover check only here at the top level */
    char *next_token = list_peek(token_iter);
    if (next_token != NULL) {
        /* gives an error if one not already given */
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Unexpected token '%s' after expression", next_token);
        }

        ast_destroy(ast);
        return NULL;

    }

    return ast;
}

/* Parses the expression using the ANDNOT operator
    this is the recursive part of the parser - helping hand from ai
    - see chatlog_1 */
static AST *parse_recursive_expression(list_iter_t *token_iter, char *errmsg) {
    /* parse the expression using the ANDNOT operator */
    AST *ast = parse_andnot(token_iter, errmsg);
    if (ast == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse expression");
        };
        return NULL;
    }

    return ast;
}

/* Parses the && term, building the ast */
static AST *parse_and(list_iter_t *tokens, char *errmsg) {
    /* parse the initial term for the left node */
    AST *left = parse_term(tokens, errmsg);
    if (left == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse left term");
        }
        return NULL;
    }

    char *peeking_token = list_peek(tokens);

    while (peeking_token != NULL && strcmp(peeking_token, "&&") == 0) {
    list_next(tokens); /* consume the "&&" token
        parse the next term for the right node */
    AST *right = parse_term(tokens, errmsg);
    if (right == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse right term");
        }
        ast_destroy(left);
        return NULL;
    }

    /* create the AST node for the AND operator */
        AST *ast_and_node = ast_create_operator(AST_AND, left, right);
        if (ast_and_node == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to create AST for AND operator");
        }
        ast_destroy(left);
        ast_destroy(right);
        return NULL;
    }

        /* update the left node to point the new AST_AND node */
        left = ast_and_node;
        peeking_token = list_peek(tokens);

    }

    return left;
}

/* Parses the || term, building the ast */
static AST *parse_or(list_iter_t *tokens, char *errmsg) {

    /* parse the AND term */
    AST *left = parse_and(tokens, errmsg);
    if (left == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse left term");
        }
        return NULL;
    }

    char *peeking_token = list_peek(tokens);
    while (peeking_token != NULL && strcmp(peeking_token, "||") == 0) {
    list_next(tokens); /* consume the "||" token
        parse the next term for the right node */
    AST *right = parse_and(tokens, errmsg);
    if (right == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse right term");
        }
        ast_destroy(left);
        return NULL;
    }

    /* create the AST node for the OR operator */
        AST *ast_or_node = ast_create_operator(AST_OR, left, right);
        if (ast_or_node == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to create AST for OR operator");
        }
        ast_destroy(left);
        ast_destroy(right);
        return NULL;
    }

        /* update the left node to point the new AST_OR node */
        left = ast_or_node;
        peeking_token = list_peek(tokens);

    }

    return left;
}

/* Parses the &! term, building the ast */
static AST *parse_andnot(list_iter_t *tokens, char *errmsg) {
    /* parse the AND term */
    AST *left = parse_or(tokens, errmsg);
    if (left == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse left term");
        }
        return NULL;
    }

    /* peek at the next token
        if its NULL or not &! we return the left node */
    char *next_token = list_peek(tokens);
    if (next_token == NULL || strcmp(next_token, "&!") != 0) {
        return left;
    }

    list_next(tokens); /* consume the "&!" token
        parse the next term for the right node */
    AST *right = parse_andnot(tokens, errmsg);
    if (right == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse right term");
        }
        ast_destroy(left);
        return NULL;
    }

    /* create the AST node for the ANDNOT operator */
    AST *ast = ast_create_operator(AST_ANDNOT, left, right);
    if (ast == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to create AST for ANDNOT operator");
        }
        ast_destroy(left);
        ast_destroy(right);
        return NULL;
    }

    return ast;
}

/* Parses a single term or a subquery, handles parentheses with grouping - help from ai */
static AST *parse_term(list_iter_t *tokens, char *errmsg) {
    char *token = list_peek(tokens);

    if (token == NULL) {
        pr_error("End of the query was reached unexpectedly\n");
        return NULL;
    }

    /* check if the token is "(" */
    if (strcmp(token, "(") == 0) {
        list_next(tokens);
        /* recursively parse the subquery */
        AST *subquery = parse_recursive_expression(tokens, errmsg);
        if (subquery == NULL) {
            if (errmsg[0] == '\0') {
                snprintf(errmsg, LINE_MAX, "Failed to parse subquery: %s", token);
            }
            return NULL;
        }

        /* after parsing we check for the end of the ")" */
        token = list_peek(tokens);
        if (token == NULL || strcmp(token, ")") != 0) {
            if (errmsg[0] == '\0') {
                snprintf(errmsg, LINE_MAX, "Expected ')' but found '%s'", token);
            }
            ast_destroy(subquery);
            return NULL;
        }

        list_next(tokens);
        return subquery;
    } else { /* note: this holdes to much power and is causing problems with a select few inputs
                at the time of writing this I dont have the time to fix this */
        /* check if the token is valid */
        token = list_next(tokens);
        if (token == NULL) {
            if (errmsg[0] == '\0') {
                snprintf(errmsg, LINE_MAX, "Expected a term but found NULL");
            }
            return NULL;
        }

        return ast_create_term(token);
    }
}



/* Utility and Debugging */

/* Traverses and evaluates the ast returning the result as a set of documents
Inspiration from https://www.reddit.com/r/C_Programming/comments/lzq2t2/how_to_make_an_ast_in_c/
got a lot of help debugging from ai and it ended up fixing and making most of this function
in the AST_TERM case
see chatlog_1 */
set_t *ast_result(AST *node, index_t *index, char *errmsg) {
    if (node == NULL) {
        snprintf(errmsg, LINE_MAX, "AST node is NULL");
        return NULL;
    }


    /* we check the type of node to be handled */
    switch (node->type) {
        case AST_TERM: {
            /* handles term node */
            if (node->data.term == NULL) {
                snprintf(errmsg, LINE_MAX, "Term is NULL");
                return NULL;
            }

            /* get the terms map */
            map_t *terms_map = index_get_terms_map(index);
            if (terms_map == NULL) {
                snprintf(errmsg, LINE_MAX, "Index terms map is NULL");
                set_t *empty_set = set_create((cmp_fn) strcmp);
                if (empty_set == NULL) {
                    snprintf(errmsg, LINE_MAX, "Failed to create empty set");
                }
                return empty_set;
            }

            /* check if it exists */
            entry_t *term_entry = map_get(terms_map, node->data.term);
            if (term_entry == NULL) {
                /* if the term is not found create a new set */
                set_t *empty_set = set_create((cmp_fn) strcmp);
                if (empty_set == NULL) {
                    pr_error("Failed to create empty set\n");
                }
                return empty_set;
            } else { /* got this from AI as my implementation did not work
                    and i couldnt get it to work see chatlog_1 */
                // Term found. Manually create a shallow copy of the set.
                set_t *original_set = (set_t *)term_entry->val;
                if (original_set == NULL) { // Should not happen, but check
                    snprintf(errmsg, LINE_MAX, "Internal error: Set for term '%s' is NULL", node->data.term);
                    return NULL; // Or return empty set
                }

                // Create a new empty set using the original set's comparison function
                cmp_fn original_cmpfn = set_get_cmpfn(original_set); // <<< Use accessor
                if (original_cmpfn == NULL) {
                    // This could happen if original_set was NULL, though checked above.
                    // Or if set_get_cmpfn fails for some reason.
                    snprintf(errmsg, LINE_MAX, "Failed to get comparison function for term '%s'", node->data.term);
                    return NULL;
                }
                set_t *result_set = set_create(original_cmpfn); // <<< Pass the retrieved function
                if (result_set == NULL) {
                    snprintf(errmsg, LINE_MAX, "Failed to create result set for term '%s'", node->data.term);
                    return NULL; // Allocation failed
                }

                // Iterate through the original set and insert items into the new set
                set_iter_t *iter = set_createiter(original_set);
                if (iter == NULL) {
                    snprintf(errmsg, LINE_MAX, "Failed to create iterator for term '%s'", node->data.term);
                    set_destroy(result_set, NULL); // Clean up the partially created set
                    return NULL;
                }

                bool insert_error = false;
                while (set_hasnext(iter)) {
                    void *item = set_next(iter);
                    // Insert the item pointer (shallow copy). set_insert returns NULL on success.
                    // We need to check the return value in case set_insert fails internally (e.g., malloc)
                    if (set_insert(result_set, item) != NULL) {
                         // Check if the element already existed (non-NULL return). This shouldn't happen
                         // if we're copying into a fresh set, unless set_insert has other failure modes
                         // or the original set had duplicates (which it shouldn't).
                         // Let's assume non-NULL means error for simplicity here, though the set interface
                         // documentation says non-NULL means "already existed". If it can fail malloc,
                         // it should ideally return a specific error or use PANIC.
                         // For now, treat any non-NULL return as an issue during copy.
                            snprintf(errmsg, LINE_MAX, "Failed to insert item into result set copy for term '%s'", node->data.term);
                            insert_error = true;
                         break; // Stop copying on error
                    }
                }
                set_destroyiter(iter); // Clean up the iterator

                if (insert_error) {
                    set_destroy(result_set, NULL); // Clean up the failed copy
                    return NULL; // Return NULL on copy failure
                }

                return result_set; // Return the newly created shallow copy
            }

        }

        /* some changes were made to all the cases with help from AI */
        case AST_AND: {
            /* handles the AND operator */
            if (node->data.children.left == NULL || node->data.children.right == NULL) {
                snprintf(errmsg, LINE_MAX, "Left or right child is NULL");
                return NULL;
            }

            /* recursively get the result of the left and right children */
            set_t *left_result = ast_result(node->data.children.left, index, errmsg);
            if (left_result == NULL) {
                snprintf(errmsg, LINE_MAX, "Failed to get left result");
                return NULL;
            }

            set_t *right_result = ast_result(node->data.children.right, index, errmsg);
            if (right_result == NULL) {
                snprintf(errmsg, LINE_MAX, "Failed to get right result");
                set_destroy(left_result, NULL);
                return NULL;
            }

            /* performe a set_intersection on the children */
            set_t *intersection = set_intersection(left_result, right_result);
            if (intersection == NULL) {
                snprintf(errmsg, LINE_MAX, "Intersection failed");
                set_destroy(left_result, NULL);
                set_destroy(right_result, NULL);
                return NULL;
            }

            set_destroy(left_result, NULL);
            set_destroy(right_result, NULL);
            return intersection;

        }


        case AST_OR: {
            /* handles the OR operator */
            if (node->data.children.left == NULL || node->data.children.right == NULL) {
                snprintf(errmsg, LINE_MAX, "Left or right child is NULL");
                return NULL;
            }

            /* recursively get the result of the left and right children */
            set_t *left_result = ast_result(node->data.children.left, index, errmsg);
            if (left_result == NULL) {
                snprintf(errmsg, LINE_MAX, "Failed to get left result");
                return NULL;
            }

            set_t *right_result = ast_result(node->data.children.right, index, errmsg);
            if (right_result == NULL) {
                snprintf(errmsg, LINE_MAX, "Failed to get right result");
                set_destroy(left_result, free);
                return NULL;
            }

            /* performe a set_union on the children */
            set_t *union_ = set_union(left_result, right_result);
            if (union_ == NULL) {
                snprintf(errmsg, LINE_MAX, "Union failed");
                set_destroy(left_result, NULL);
                set_destroy(right_result, NULL);
                return NULL;
            }

            set_destroy(left_result, NULL);
            set_destroy(right_result, NULL);
            return union_;
        }


        case AST_ANDNOT: {
            /* handles the ANDNOT operator - done by autocomplete */
            if (node->data.children.left == NULL || node->data.children.right == NULL) {
                snprintf(errmsg, LINE_MAX, "Left or right child is NULL");
                return NULL;
            }

            /* recursively get the result of the left and right children */
            set_t *left_result = ast_result(node->data.children.left, index, errmsg);
            if (left_result == NULL) {
                snprintf(errmsg, LINE_MAX, "Failed to get left result");
                return NULL;
            }

            set_t *right_result = ast_result(node->data.children.right, index, errmsg);
            if (right_result == NULL) {
                snprintf(errmsg, LINE_MAX, "Failed to get right result");
                set_destroy(left_result, NULL);
                return NULL;
            }

            /* performe a set_difference on the children */
            set_t *difference = set_difference(left_result, right_result);
            if (difference == NULL) {
                snprintf(errmsg, LINE_MAX, "Difference failed");
                set_destroy(left_result, NULL);
                set_destroy(right_result, NULL);
                return NULL;
            }

            set_destroy(left_result, NULL);
            set_destroy(right_result, NULL);
            return difference;
        }
        
        default:
            snprintf(errmsg, LINE_MAX, "Invalid AST node type");
            return NULL;
    }
    
}