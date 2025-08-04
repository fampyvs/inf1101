/**
 * @implements index.h
 */

/* set log level for prints in this file */
#define LOG_LEVEL LOG_LEVEL_DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h> 

#include "printing.h"
#include "index.h"
#include "defs.h"
#include "common.h"
#include "list.h"
#include "map.h"
#include "set.h"
#include "ast.h"


struct index {
    map_t *terms;
    set_t *docs;
    size_t n_docs;
    size_t n_terms;
    map_t *term_frequency;
    map_t *doc_term_count; 
};

/**
 * You may utilize this for lists of query results, or write your own comparison function.
 */
ATTR_MAYBE_UNUSED
int compare_results_by_score(query_result_t *a, query_result_t *b) {
    if (a->score > b->score) {
        return -1;
    }
    if (a->score < b->score) {
        return 1;
    }
    return 0;
}





/* Helper Functions for index_destroy */

/* destroys the set stored in index->terms */
static void free_term_set(void *set_ptr) {
    if (set_ptr) {
        set_destroy(set_ptr, NULL);
    }
}


/* destroys the values in index->term_frequency
    this has ownership of its keys and values */
static void free_term_freq_map(void *map_ptr) {
    if (map_ptr) {
        map_destroy(map_ptr, free, free);
    }
}


/* frees the size_t value in index_doc_term_count
     */
static void free_size_t_ptr(void *ptr) {
    free(ptr);
}



index_t *index_create(cmp_fn cmpfn, hash64_fn hashfn) {
    index_t *index = malloc(sizeof(index_t));
    if (index == NULL) {
        pr_error("Failed to allocate memory for index\n");
        return NULL;
    }

    /* initialize count for docs and terms*/
    index->terms = NULL;
    index->docs = NULL;
    index->term_frequency = NULL;
    index->doc_term_count = NULL;
    index->n_docs = 0;
    index->n_terms = 0;

    /* createe the map to store the index */
    index->terms = map_create(cmpfn, hashfn);
    if (index->terms == NULL) {
        pr_error("Failed to create map for index\n");
        free(index);
        return NULL;
    }

    /* create the set that will store the documents */
    index->docs = set_create((cmp_fn) strcmp);
    if (index->docs == NULL) {
        pr_error("Failed to create set for index\n");
        map_destroy(index->terms, NULL, NULL);
        free(index);
        return NULL;
    }

    /* create a map for the terms frequency */
    index->term_frequency = map_create(cmpfn, hashfn);
    if (index->term_frequency == NULL) {
        pr_error("Failed to create map for term frequency\n");
        map_destroy(index->terms, NULL, NULL);
        set_destroy(index->docs, NULL);
        free(index);
        return NULL;
    }

    /* create a map for the document term count
        that is per document */
    index->doc_term_count = map_create(cmpfn, hashfn);
    if (index->doc_term_count == NULL) {
        pr_error("Failed to create map for document frequency\n");
        map_destroy(index->terms, NULL, NULL);
        set_destroy(index->docs, NULL);
        map_destroy(index->term_frequency, NULL, NULL);
        free(index);
        return NULL;
    }


    return index;
}


void index_destroy(index_t *index) {
    if (index == NULL) {
        return;
    }

    /* destroys the terms, keys and values owned by the map */
    if (index->terms) {
        map_destroy(index->terms, free, free_term_set);
    }

    /* destroys the term_frequency in index
        keys owned index->docs (NULL), values owned by map */
    if (index->term_frequency) {
        map_destroy(index->term_frequency, NULL, free_term_freq_map);
    }

    /* destroy doc_term_count in index
        keys are owned by index->docs (NULL), values owned by map,
        value should be malloced size _T*/
    if (index->doc_term_count) {
        map_destroy(index->doc_term_count, NULL, free_size_t_ptr);
    }

    /* destroy the docs in index
        items owned by the set */
    if (index->docs) {
        set_destroy(index->docs, free);
    }

    free(index);
}




int index_document(index_t *index, char *doc_name, list_t *terms) {
    if (index == NULL || doc_name == NULL || terms == NULL) {
        pr_error("Arguments cannot be NULL\n");
        return -1;
    }


    /* add document to the docs set, duplicate the sting */
    char *doc_copy = strdup(doc_name);
    if (doc_copy == NULL) {
        pr_error("Failed to duplicate document name\n");
        return -1;
    }

    /* add this to docs */
    set_insert(index->docs, doc_copy);
    index->n_docs += 1;

    /* create a map to store the frequancy in */
    map_t *term_freq_doc = map_create((cmp_fn) strcmp, (hash64_fn) hash_string_fnv1a64);
    if (term_freq_doc == NULL) {
        pr_error("Failed to create map for term frequency\n");
        free(doc_copy);
        return -1;
    }

    /* add the name and freq to the index */
    map_insert(index->term_frequency, doc_copy, term_freq_doc);


    /* iterate through the terms list*/
    list_iter_t *terms_iter = list_createiter(terms);
    if (terms_iter == NULL) {
        pr_error("Failed to create iterator for terms\n");
        free(doc_copy);
        return -1;
    }

    size_t current_doc_term_count = 0; /* used to count the number of terms in the document */

    /* iterate throught the list for each term in the list
        got help from ai debugging see chatlog_1 */
    while (list_hasnext(terms_iter)) {
        char *term = list_next(terms_iter);
        if (term == NULL) {
            pr_error("Failed to get next term from list\n");
            continue;
        }

        /* check if the term is a stop word
            if it returns true we start over with a new term
            if it returns false we process that term */
        if (stop_word(term)) {
            continue;
        }

        current_doc_term_count++; /* increment the term count for this document */
        
        

        /* check if the term is already in the map */
        entry_t *entry = map_get(index->terms, term);
        if (entry == NULL) {

            /* create a seperate copy spcifically for insetion - help from ai */
            char *term_key_for_index = strdup(term);
            if (term_key_for_index == NULL) {
                pr_error("Failed to duplicate term string\n");
                continue;
            }

            /* create a new set to store the doc name */
            set_t *docs_set = set_create((cmp_fn) strcmp);
            if (docs_set == NULL) {
                pr_error("Failed to create set for term\n");
                free(term_key_for_index);
                continue;
            }
            /* add the duplicate doc to the set */
            set_insert(docs_set, doc_copy);
            /* add the duplicate term to the map */
            map_insert(index->terms, term_key_for_index, docs_set);
            index->n_terms += 1; /* increment the number of terms */
        }

        /* if the term does exist in the map */
        else {
            /* get the existing set of doc names for this term */
            set_t *existing_set = entry->val;

            /* add the duplicate doc to the existing set */
            set_insert(existing_set, doc_copy);
        }

        /* update the term frequency for this document */
        entry_t *term_freq = map_get(term_freq_doc, term);
        if (term_freq == NULL) {

            /* create a seperate copy for TF doc key - help from ai */
            char *term_key_for_tf = strdup(term);
            if (term_key_for_tf == NULL) {
                pr_error("Failed to duplicate term string for term frequency\n");
                continue;
            }


            /* create a new term count */
            uint32_t *new_term_count = malloc(sizeof(uint32_t));
            if (new_term_count == NULL) {
                pr_error("Failed to allocate memory for term frequency\n");
                free(term_key_for_tf);
                continue;
            }
            *new_term_count = 1; /* set the frequency to 1 */
            map_insert(term_freq_doc, term_key_for_tf, new_term_count);
        } 
        
        else {
            /* increment the existing term frequency */
            uint32_t *term_freq_value = term_freq->val;
            *term_freq_value += 1; /* increment the frequency */
        }
        

    }

    /* store the total count of term for y document
        this is used later to count the tf-idf */
    size_t *total_doc_term_count = malloc(sizeof(size_t));
    if (total_doc_term_count == NULL) {
        pr_error("Failed to allocate memory for total doc term count\n");
        free(doc_copy);
        map_destroy(term_freq_doc, free, free);
        return -1;
    }
    *total_doc_term_count = current_doc_term_count;

    /* insert the total count into the map */
    map_insert(index->doc_term_count, doc_copy, total_doc_term_count);

    list_destroyiter(terms_iter);


    return 0;
}

/* returns the terms - from ai */
map_t *index_get_terms_map(index_t *index) {
    if (index == NULL) {
        return NULL;
    }
    return index->terms;
}

/* got some help from ai with this */

list_t *index_query(index_t *index, list_t *query_tokens, char *errmsg) {
    
    if (index == NULL || query_tokens == NULL) {
        pr_error("Arguments cannot be NULL\n");
        return NULL;
    }

    /* parse the query tokens into the ast */
    list_iter_t *tokens_iter = list_createiter(query_tokens);
    if (tokens_iter == NULL) {
        pr_error("Failed to create iterator for query tokens\n");
        return NULL;
    }

    /* parse the expression */
    AST *ast = parse_expression(tokens_iter, errmsg);
    if (ast == NULL) {
        if (errmsg[0] == '\0') {
            snprintf(errmsg, LINE_MAX, "Failed to parse query expression");
        }
        list_destroyiter(tokens_iter);
        return NULL;
    }
    list_destroyiter(tokens_iter);

    /* call for the ast_results to get matching documents */
    set_t *result_set = ast_result(ast, index, errmsg);
    if (result_set == NULL) {
        snprintf(errmsg, LINE_MAX, "Failed to get result set");
        ast_destroy(ast);
        return NULL;
    }

    /* create a list to store the results */
    list_t *result_list = list_create((cmp_fn) compare_results_by_score);
    if (result_list == NULL) {
        snprintf(errmsg, LINE_MAX, "Failed to create result list");
        set_destroy(result_set, free);
        ast_destroy(ast);
        return NULL;
    }

    /* then iterate through the list of results for each document */
    set_iter_t *result_iter = set_createiter(result_set);
    if (result_iter == NULL) {
        snprintf(errmsg, LINE_MAX, "Failed to create iterator for result set");
        set_destroy(result_set, free);
        ast_destroy(ast);
        list_destroy(result_list, free);
        return NULL;
    }
    while (set_hasnext(result_iter)) {
        char *doc_name = set_next(result_iter);
        if (doc_name == NULL) {
            snprintf(errmsg, LINE_MAX, "Failed to get next document name");
            continue;
        }

        /* create a new query result */
        query_result_t *result = malloc(sizeof(query_result_t));
        if (result == NULL) {
            snprintf(errmsg, LINE_MAX, "Failed to allocate memory for query result");
            continue;
        }

        /* set the document name and score */
        result->doc_name = strdup(doc_name);
        if (result->doc_name == NULL) {
            snprintf(errmsg, LINE_MAX, "Failed to duplicate document name");
            free(result);
            continue;
        }
        
        result->score = calculate_tfidf(index, ast, doc_name); /* calculate the score */

        /* add the result to the list */
        if (list_addlast(result_list, result) < 0) {
            snprintf(errmsg, LINE_MAX, "Failed to add result to list");
            free(result->doc_name);
            free(result);
            set_destroyiter(result_iter);
            set_destroy(result_set, free);
            ast_destroy(ast);
            list_destroy(result_list, free);
            return NULL;
        }
    }

    /* sort the result list */
    list_sort(result_list);

    /* cleanup and return */
    set_destroyiter(result_iter);
    set_destroy(result_set, NULL);
    ast_destroy(ast);
    return result_list;


}

void index_stat(index_t *index, size_t *n_docs, size_t *n_terms) {
    if (index == NULL || n_docs == NULL || n_terms == NULL) {
        pr_error("Arguments cannot be NULL\n");
        return;
    }

    /* get the number of documents and terms in the index */
    *n_docs = index->n_docs;
    *n_terms = index->n_terms;

    /* print the number of documents and terms - from autocomplete */
    pr_info("Number of documents: %zu\n", *n_docs);
    pr_info("Number of terms: %zu\n", *n_terms);

}


/* calculates the TF-IDF recursively for each term in the document
with help from https://en.wikipedia.org/wiki/Tf%E2%80%93idf and
https://www.geeksforgeeks.org/understanding-tf-idf-term-frequency-inverse-document-frequency/
and got help from AI, in regards to compilation errors */
double calculate_tfidf(index_t *index, AST *ast, char *doc_name) {

    if (ast == NULL) {
        pr_error("AST node is NULL\n");
        return 0.0;
    }

    double tfidf_score = 0.0;

    switch (ast->type) {
        case AST_TERM: {
            /* -- TF -- */

            /* get the maps entry containing the docs term */
            entry_t *TF_map_entry = map_get(index->term_frequency, doc_name);
            if (TF_map_entry == NULL) {
                return 0.0;
            }

            /* gget the actual value from the map */
            map_t *doc_term_count = TF_map_entry->val;
            if (doc_term_count == NULL) {
                return 0.0;
            }

            /* get the total number of term in y doc (doc_name) */
            entry_t *total_entry_count = map_get(index->doc_term_count, doc_name);
            if (total_entry_count == NULL) {
                return 0.0;
            }
            /* get the total number of terms in the document */
            size_t *total_num_term_ptr = total_entry_count->val;
            if (total_num_term_ptr == NULL) { 
                return 0.0;
            }

            /* this holds the value for the docs */
            size_t *total_doc_term_count = total_num_term_ptr;
            if (total_doc_term_count == 0) {
                return 0.0;
            }

            /* calculate the TF for one term */
            double tf = 0.0;

            /* get the term count for the term in the document */
            entry_t *term_count_entry = map_get(doc_term_count, ast->data.term);
            if (term_count_entry == NULL) {
                return 0.0;
            }

            /* get the actual value from the map */
            uint32_t *term_count_ptr = term_count_entry->val;
            if (term_count_ptr == NULL) {
                return 0.0;
            }
            uint32_t *raw_term_count = term_count_ptr;

            
            tf = (double)*raw_term_count / (double)*total_doc_term_count;


            /* -- IDF / DF -- */

            /* get the x term entry from the main term map */
            entry_t *term_entry = map_get(index->terms, ast->data.term);
            if (term_entry == NULL) {
                return 0;
            }
            /* get the total number of documents in the index */
            size_t N = index->n_docs;

            /* get all the documents containing x term */
            set_t *doc_term = term_entry->val;
            if (doc_term == NULL) {
                return 0;
            }

            /* get the number of documents in the set */
            size_t Df = set_length(doc_term);
            

            double idf = 0;

            /* calculate the IDF for one term */
            idf = log((double)N / (double)Df);
            


            /* -- TF-IDF -- */

            /* calculate the TF-IDF for one term */
            tfidf_score = tf * idf;
            break;
        }

        case AST_AND: {
            double left_result = calculate_tfidf(index, ast->data.children.left, doc_name);
            double right_result = calculate_tfidf(index, ast->data.children.right, doc_name);

            tfidf_score = left_result + right_result;
            break;
        }

        case AST_OR: {
            double left_result = calculate_tfidf(index, ast->data.children.left, doc_name);
            double right_result = calculate_tfidf(index, ast->data.children.right, doc_name);

            tfidf_score = left_result + right_result;
            break;
        }

        case AST_ANDNOT: {
            tfidf_score = calculate_tfidf(index, ast->data.children.left, doc_name);
            break;
        }
    
        default:
            pr_error("Unknown AST type\n");
            tfidf_score = 0;
            break;
    }

    return tfidf_score;


}


const char *stop_words[] = { /* got help from autocomplete with the words - but if you want to add more words
                                you need to sort them alphabetically */
    "about", "all", "an", "and", "any", "are", "as", "at", "be", "been", "being",
    "but", "by", "did", "do", "does", "else", "for", "from", "had", "has", "have", "here", 
    "how", "if", "in", "is", "like", "more", "most", "no", "none", "not",
    "of", "on", "or", "some", "such", "than", "that", "the", "then",
    "there", "these", "this", "those", "to", "was", "were", "what", "when", "where",
    "which", "who", "whom", "whose", "why", "with",
    NULL // NULL-terminated list
};

/* inspired from https://www.geeksforgeeks.org/binary-search-a-string/
    could probably be done differently, but for this implementation its fine
    
    returns true if a stop word was found, false if it wasnt */
bool stop_word(const char *term) {
    int left = 0;
    int right = sizeof(stop_words) / sizeof(stop_words[0]) - 2;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        int cmp = strcmp(term, stop_words[mid]);
        if (cmp == 0) {
            return true; 
        } else if (cmp > 0) {
            left = mid + 1; 
        } else {
            right = mid - 1;
        }
    }

    return false;
}