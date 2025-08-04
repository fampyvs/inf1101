/**
 * @authors
 * Odin Bjerke <odin.bjerke@uit.no>
 */

#include "common.h"
#include "futil.h"
#include "printing.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <errno.h>
#include <ctype.h>


typedef struct word_freq {
    char *word;
    size_t count;
} word_freq_t;



int compare_word_freq_by_count(word_freq_t *a, word_freq_t *b) {
    /* switch 1 and -1 to output in ascending order */
    if (a->count > b->count) {
        return -1;
    }
    if (a->count < b->count) {
        return 1;
    }
    return 0;
}

void word_freq_free(word_freq_t *freq) {
    free(freq->word);
    free(freq);
}

static int list_sort_and_verify(list_t *list, cmp_fn cmpfn) {
    size_t llen = list_length(list);
    list_sort(list);
    size_t llen_after = list_length(list);

    if (llen != llen_after) {
        pr_error("List was mutated by sort. Length before sort = %zu, after = %zu\n", llen, llen_after);
        return -1;
    }

    if (llen < 2) {
        pr_warn("List is sorted, but too short (%zu) to verify whether list_sort works as intended\n", llen);
        return 1;
    }

    list_iter_t *iter = list_createiter(list);
    if (!iter) {
        pr_error("failed to create list iterator\n");
        return -1;
    }

    if (!list_hasnext(iter)) {
        pr_error("list_hasnext returned 0 for new iterator on list of length %zu\n", llen);
        list_destroyiter(iter);
        return -1;
    }

    int issorted = 1;
    void *prev = list_next(iter);

    while (issorted == 1 && list_hasnext(iter)) {
        void *curr = list_next(iter);

        /* the lists in this file will never insert a NULL-item into the list, so utilize that to verify */
        if (prev == NULL || curr == NULL) {
            pr_error("unexpected NULL-item before end of list\n");
            issorted = -1;
        } else {
            int cmp = cmpfn(curr, prev);

            if (cmp < 0) {
                pr_error("List of length %zu is not sorted correctly (curr < prev)\n", llen);
                issorted = 0;
            } else if (cmp > 0) {
                prev = curr;
            } else if (!list_hasnext(iter)) {
                /* equal to previous item */
                pr_debug("List of length %zu appears to have been sorted correctly\n", llen);
            }
        }
    }

    list_destroyiter(iter);

    return issorted;
}

static list_t *create_wordfreqs_list(list_t *words) {
    list_t *freqs = list_create((cmp_fn) compare_word_freq_by_count);
    list_iter_t *words_iter = list_createiter(words);

    if (!freqs) {
        pr_error("Failed to create list\n");
        goto err_cleanup;
    }
    if (!words_iter) {
        pr_error("failed to create list iterator\n");
        goto err_cleanup;
    }

    word_freq_t *freq = NULL;

    while (list_hasnext(words_iter)) {
        char *word = list_next(words_iter);

        if (freq && strcmp(freq->word, word) == 0) {
            freq->count++;
            continue;
        }

        freq = malloc(sizeof(word_freq_t));
        if (!freq) {
            pr_error("Cannot allocate memory\n");
            goto err_cleanup;
        }

        freq->count = 1;
        freq->word = strdup(word);

        if (!freq->word) {
            pr_error("Cannot allocate memory\n");
            free(freq);
            goto err_cleanup;
        }
        if (list_addfirst(freqs, freq) < 0) {
            pr_error("list_addfirst failed\n");
            word_freq_free(freq);
            goto err_cleanup;
        }
    }

    list_destroyiter(words_iter);

    if (list_sort_and_verify(freqs, (cmp_fn) compare_word_freq_by_count) == 1) {
        return freqs;
    }

    pr_error("Failed to sort list of word_freq structs\n");

err_cleanup:
    if (words_iter) {
        list_destroyiter(words_iter);
    }
    if (freqs) {
        list_destroy(freqs, (free_fn) word_freq_free);
    }

    return NULL;
}

static int print_wordfreqs_list(list_t *freqs, size_t min_wc, size_t lim_nres) {
    list_iter_t *freqs_iter = list_createiter(freqs);
    if (!freqs_iter) {
        pr_error("failed to create iterator\n");
        return -1;
    }

    printf("Number of distinct words: %zu\n\n", list_length(freqs));
    printf("--- Words that occured at least %zu times", min_wc);
    if (lim_nres) {
        printf(" (limiting to max %zu results)", lim_nres);
    }
    printf(" ---\n");
    printf("%-30s   %s\n", "TERM", "COUNT");

    size_t n_printed = 0;

    while (list_hasnext(freqs_iter) && (lim_nres == 0 || n_printed < lim_nres)) {
        word_freq_t *freq = list_next(freqs_iter);
        if (freq->count >= min_wc) {
            printf("%-30s | %zu\n", freq->word, freq->count);
            n_printed++;
        }
    }
    list_destroyiter(freqs_iter);

    return 0;
}

static int parse_args(int argc, char **argv, char **fpath, size_t *min_wc, size_t *min_wl, size_t *lim_nres) {
    /* clang-format off */
    if (argc != 5) {
        pr_error("Missing one or more required positional argument\n");
        fprintf(stderr, "Usage: ./%s <fpath> <min_wc> <min_wl> <lim_n_results>\n", basename(argv[0]));
        fprintf(stderr, "* <fpath>: Path to a readable file. The file will never be modified.\n");
        fprintf(stderr, "* <min_wc>: Exclude words that occur less times than this value. 1 to include all.\n");
        fprintf(stderr, "* <min_wl>: Exclude words shorter than this value. 1 to include all.\n");
        fprintf(stderr, "* <lim_n_results>: Print at most this many results. 0 to print all.\n");
        fprintf(stderr, "---\n");
        fprintf(stderr, "Example A: %s src/wordfreq.c 10 2 10\n", argv[0]);
        fprintf(stderr, "Example B: %s data/oxford_dict.txt 1 13 25\n", argv[0]);
        fprintf(stderr, "Example C: make run ARGS=\"data/oxford_dict.txt 100 4 25\"\n");
        return -1;
    }
    /* clang-format on */

    errno = 0;

    long min_wc_ = strtol(argv[2], NULL, 10);
    if (errno) {
        pr_error("Bad argument \"%s\" for <min_wc>: %s\n", argv[2], strerror(errno));
        return -2;
    }

    long min_wl_ = strtol(argv[3], NULL, 10);
    if (errno) {
        pr_error("Bad argument \"%s\" for <min_wl>: %s\n", argv[3], strerror(errno));
        return -2;
    }

    long lim_nres_ = strtol(argv[4], NULL, 10);
    if (errno) {
        pr_error("Bad argument \"%s\" for <lim_n_results>: %s\n", argv[4], strerror(errno));
        return -2;
    }

    *fpath = argv[1];
    *min_wc = (min_wc_ < 1) ? 1 : (size_t) min_wc_;
    *min_wl = (min_wl_ < 1) ? 1 : (size_t) min_wl_;
    *lim_nres = (lim_nres_ < 0) ? 0 : (size_t) lim_nres_;

    return 0;
}

int main(int argc, char **argv) {
    char *fpath;
    size_t min_wc, min_wl, lim_nres;

    int rc = parse_args(argc, argv, &fpath, &min_wc, &min_wl, &lim_nres);
    if (rc < 0) {
        return EXIT_FAILURE;
    }

    FILE *infile = fopen(fpath, "r");
    if (!infile) {
        pr_error("Failed to open %s: %s\n", fpath, strerror(errno));
        return EXIT_FAILURE;
    }

    list_t *words = list_create((cmp_fn) strcmp);
    if (!words) {
        pr_error("Failed to create list\n");
        fclose(infile);
        return EXIT_FAILURE;
    }

    /* tokenize: split at whitespace, include only alphanumeric chars, convert to lowercase */
    rc = ftokenize(infile, words, min_wl, isspace, isalnum, tolower);

    if (rc >= 0 && list_length(words)) {
        if (list_sort_and_verify(words, (cmp_fn) strcmp) != 1) {
            pr_error("Failed to sort list of words\n");
            rc = -1;
        } else {
            list_t *freqs = create_wordfreqs_list(words);

            if (freqs) {
                printf("\n--- %s | Words consisting of at least %zu chars ---\n", basename(fpath), min_wl);
                printf("Total number of words: %zu\n", list_length(words));

                rc = print_wordfreqs_list(freqs, min_wc, lim_nres);
                list_destroy(freqs, (free_fn) word_freq_free);
            } else {
                rc = -1;
            }
        }
    }

    fclose(infile);
    list_destroy(words, free);

    return (rc < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}
