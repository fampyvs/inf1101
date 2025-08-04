# Overview 💡

This is the first of two mandatory assignments. 

In this assignment, you are to implement a index ADT. The index should support basic search operations such as getting, removing and counting elements. The map will be used for a word frequency application, where we can search for a word and get the number of occurrences of that word in the provided file.

# Algorithms 🧮

How you implement the index is up to you.
The index requires two components to function:
1. A linked list implementation, as specified by `list.h`
2. A data structure to store arbitrary values searchable by keys.

The exposed interface, is the `map.h` file. The application we have provided only cares about these functions, and what data structure you choose to implement for storing and retrieving the actual words is your choice to make.

The requirements for your chosen data structure is that:

- The *average complexity* for search should be **O(log n)**. You should prove that your implementation fulfils this in your report through experiments/benchmarks.
- Your data structure should comply with the interface given by `map.h`

In essence you will need to implement a form of *associative array* or *map*. Common choices would be a Hash Table or Binary Search Tree. 


# Applications 💻

To test your list implementation, we have given you a word search application.
The application counts the occurrences of words within a document, and provides a search CLI to search for words and their frequency. We have also provided you a copy of the Oxford Dictionary to test the application with.

We have also provided `testmap` and `testlist` files, that runs a series of tests on your `map` and `list` implementations. Note that these tests do not guarantee that your implementation is correct, but they give a good indication that it seems to satisfy the provided `map` and `list` interfaces.



# Code 👩‍💻

You can find the precode here.

```
p1-pre/
├── data/
│   └── oxford_dict.txt - Dictionary to test your application
├── include/
│   ├── common.h - Common utilities
│   ├── defs.h - Function pointer definitions
│   ├── futil.h - File utilities
│   ├── list.h - List interface
│   ├── map.h - Map interface
│   ├── printing.h - Printing utilities
│   └── testmap.h - Test functions
│   └── ... Add your data structure .h files here
├── src/
│   ├── common.c - Common utilities
│   ├── futil.c - File utilities
│   ├── linkedlist.c - Linked list implementation (IMPLEMENT THIS)
│   ├── map.c - Map implementation (IMPLEMENT THIS)
│   ├── main.c - The main program (MODIFY THIS)
│   └── testmap.c - Test functions for the map
│   └── ... Add your data structure .c files here 
├── p1.md 
├── README.md
└── Makefile - Makefile for the project (MODIFY THIS IF NEEDED)
```

You are free to add any files you need to implement your map interface.

You will also need to add benchmarking/experiments to gather data from your map implementation to use in your report.

**Beware that the application may not compile or produce a lot of warnings before your have implemented the list and map structures.**

# Report

In addition to your code, you will need to write and submit a technical report that explains your design. The report should contain these elements:

- **Introduction** - Introduce the solution that you have come up with
- **Design/Methods** - Describe how and what has been 
- **Results** - Describe your experiments and the results from these
- **Discussion** - Discuss your results
- **Conclusion** - Conclude your report

The most crucial part of the report is the results and discussion. As mentioned, you will need to prove through experiments that your `map` implementation has **O(log n)** time complexity, and the experimental data should be presented in a readable form.

# Deadline ✔️🕛🚩

The deadline for the assignment 10.03.25 before 13:00.
This is a hard deadline. Any submission after this time will be an automatic fail.

To get a new deadline, please contact your TA (for your colloquium group), or one of the course lecturers. Any extension based on medical reasons, needs to be documented and applied for with the administration of the faculty (*not the course staff*).

*Remember, you are only given 2 attempts at a mandatory assignment. This means that a **late submission** fail will count as one of the attempts, and you will not get any feedback. It is better to submit **something** before the deadline, even if you expect a fail grade, because you will still get feedback on what needs to be improved.*

# Requirements summary

In summation, the requirements for this assignment are:

- Implement a *linked list* satisfying the `list.h` interface
- Implement a *map* satisfying the `map.h` interface with a average search complexity of O(log n)
- Design and implement experiments/benchmarks and gather data about your maps performance
- Write a report and present your design, and present your experimental results.
- Argue based on your results that the implementation satisfies the requirement for average search time complexity
- Argue for what trade-offs your application makes with regards to space and time complexity

# Deliverables

Submit your code as a .zip or tarball with the following signature: abc012-p1.zip were you replace the letters and numbers with your UiT Username. The zip file should contain the following folder structure:

```
├── abc012-p1/ - Your UiT username
   └── Makefile - Makefile for the project
   └── README.md - Explanation of how to compile and run your project
   └── run.sh - Run script to launch the application with standard arguments
   ├── doc/
      └── report.pdf - Your report goes here
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

**Submissions not adhering to these guidelines may be rejected**
