# wordfreq
Simple command line program to list the frequency of words in a given input file.  

# Directory Structure
```
p1
  `--src
  `--include
  `--data
  `--obj
  `--bin
     `--debug
     `--release
```

* `src/` - source files for the main program and dependancies
* `include/` - header files, specifying various interface/APIs
* `data/` - data files meant to be utilized by (or produced by) a program
* `bin/` - output directory for compiled binary targets. Running `make distclean` will remove this folder.


# Compiling the program
A makefile is provided to simplify compiling the program. The makefile provides two configurations for compiling the program, _debug_ and _release_.  

## _debug_
Compile with `make` or `make exec`.  
By default, the makefile is set to enable compile-time warnings and symbolic information nescessary for program such as gdb and lldb.
This mode is default for a reason. Unless you are certain the program works as intended, utilize this option.

## _release_
If `DEBUG=0` is set in the makefile, or overridden by `make DEBUG=0`, the program will instead be compiled with minimal warnings and a high degree of optimization.  
The compiled executable will be placed in `bin/release` and object dependencies will be placed in `obj/`. 

Building in release-mode also directs the compiler to completely remove:
* All `printing.h` invocations except for `pr_error` and `PANIC`
* All assertions, either through `assert.h` or `printing.h`


# Usage
`./<exec_path> <fpath> <min_wc> <min_wl> <lim_n_results>`

## Runtime Arguments
* `<fpath>`  
Path to a readable file. The file will never be modified.
* `<min_wc>`  
Exclude words that occur less times than this value. 1 to include all.
* `<min_wl>`  
Exclude words shorter than this value. 1 to include all.
* `<lim_n_results>`  
Print at most this many results. 0 to print all.

## Example Usage
* `./bin/debug/wordfreq.out src/wordfreq.c 10 2 10`  
    * Specify `src/wordsfreq.c` as the file to read by the program
    * Include only words from the file that occur 10 or more times
    * Include only words from the file that are 2 or more characters long
    * Limit to max 10 results
* `./bin/debug/wordfreq.out data/oxford_dict.txt 1 13 25`  
    * Specify `data/oxford_dict.txt` as the file to read by the program
    * Include words that occur at least once (all words)
    * Include only words from the file that are 13 or more characters long
    * Limit to max 25 results
* `sh run.sh`  
    * Runs either `bin/debug` or `bin/release`, whichever is available.
    * Include words that occur at least once (all words)
    * Include only words from the file that are 4 or more characters long
    * Limit to max 10 results


# Testing your solution
The text file at `data/oxford_dict.txt` is a text file containing roughly 30000 entries (418737 words) from the oxford english dictionary, and may be utilized to test the program. Any files containing text are accepted, however, as demonstrated above where the program is given its own source file, `wordfreq.c`, as input.

The program will not work correctly unless the list implementation is working. An explicit check is performed to verify whether a call to `list_sort` actually sorted the list in question. 

If the list implementation is working correctly, running the program with the args `data/oxford_dict.txt 1 4 10` (the arguments utilized by `run.sh`) will result in the following output:

```log
--- oxford_dict.txt | Words consisting of at least 4 chars ---
Total number of words: 418737
Number of distinct words: 56333

--- Words that occured at least 1 times (limiting to max 10 results) ---
TERM                             COUNT
latin                          | 6293
with                           | 6051
from                           | 4858
related                        | 3790
person                         | 3479
foll                           | 3323
colloq                         | 3083
french                         | 2356
often                          | 2231
english                        | 2094
```


# printing.h
Provides several printf-like macros that may be utilized:
```c
pr_error(fmt, ...)
pr_warn(fmt, ...)
pr_info(fmt, ...)
pr_debug(fmt, ...)
PANIC(fmt, ...)
assert(assertion)
assertf(assertion, fmt, ...)
```

The compile-time definition of `LOG_LEVEL` controls what prints are included in the compiled program:
```c
enum {
    LOG_LEVEL_PANIC = 0, /* minimum log level. only prints on PANIC. */
    LOG_LEVEL_ERROR,     /* enable pr_error */
    LOG_LEVEL_WARN,      /* enable pr_warn (+ pr_error) */
    LOG_LEVEL_INFO,      /* enable pr_info (+ pr_error, pr_warn) */
    LOG_LEVEL_DEBUG      /* enable pr_debug (+ pr_error, pr_warn, pr_info) */
};
```
If no compile-time definition of `LOG_LEVEL` is provided, it defaults to `LOG_LEVEL_DEBUG`.  

The following options are also available:
* remove colors (define `PRINTING_NCOLOR`)
* removal of meta information such as file/line. (define `PRINTING_NMETA`)

If you prefer utilizing typical `printf` calls, you are obviously free to do so. However, having the ability to toggle a category of prints by a single definition; rather than commenting them out, is quite convenient.
