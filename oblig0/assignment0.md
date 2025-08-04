# Overview 💡


This is an optional, but highly recommended, assignment. It will give you a head start on Assignment 1 (where you will need to implement parts of assignment 0).

In this assignment, you are to implement a linked list. The list ADT specifies basic list operations such as adding, removing, iterating and sorting the elements.

# Algorithms 🧮

You can implement any type of list you'd like, as long as it satisfies the interface specified by list.h.
We recommend that you try implementing a doubly linked list, but it is not a requirement.
A doubly linked list is more or less the same as a singly, but with prev-pointers in each node.

To make the wordfreq application work, the list needs to be able to be sorted.
We have provided a mergesort implementation which will work with both singly and doubly linked lists,
but for other types of list you need to implement your own sorting algorithm.

# Applications 💻

To test your list implementation, we have given you a word frequency application. The application counts the occurrences of words within a document, and lists out the N most common words and their frequency. We have also provided you a copy of the Oxford Dictionary to test the application with.

# Code 👩‍💻

You can find the precode here. It consists of the following files:

```
├── p1-pre/
   ├── data/
      └── oxford_dictionary.txt - Sample data to use with the wordfreq application
   ├── include/
      └── common.h - Common utility functions interface
      └── futil.h - File interaction utilities interface
      └── list.h - List interface (IMPLEMENT THESE)
      └── printing.h - Printing utilities
   ├── src/
      └── common.c - Common utility function implementations
      └── futil.c - File interaction utilities implementations
      └── linkedlist.c - Stub file for your list. WRITE YOUR LIST CODE HERE.
      └── main.c - Main wordfreq application.
   └── Makefile - Makefile for the project. Contains one target (all)
   └── README.md - Instructions for compiling and running the application.
   └── run.sh - Shell script to run the application with a set of arguments.
```

You might be a bit overwhelmed by the amount of code, but make sure to read the README.md file. In essence you only need to read one file (`list.h`) and implement another (`linkedlist.c`).

If you want to implement multiple different versions of the list, you are free to do so. But beware for your implementation to work with the wordfreq application, it needs to satisfy the interface given by `list.h`.

# Deadline ✔️🕛🚩

You can submit the assignment before January 27th to receive some feedback on your implementation. Any assignment submitted after the deadline will not be reviewed.
Deliverables

Submit your code as a .zip or tarball with the following signature: abc012-p1.zip were you replace the letters and numbers with your UiT Username. The zip file should contain the following folder structure:

```
├── abc012-p1/
   └── Makefile
   └── README.md
   └── run.sh
   ├── src/
      └── sourcefiles.c
      └── ...
   ├── include/
      └── headerfiles.h
      └── ...
   ├── data/
      └── anydata.txt
      └── ...
```

Make sure you run make distclean before you compress and submit your assignment.
