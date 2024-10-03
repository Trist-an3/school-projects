## Programming Assignment 4
* Name: Tristan Garcia
* CruzID: tgarcia7
* StudentID: 1991617

## Purpose:
    building two concurrent data structures a bounded buffer (FIFO queue) and a reader-writer lock.
    The queue allows multiple producers and consumers to access it simultaneously. The reader-writer
    lock allows multiple readers to hold the lock simultaneously but only one writer, with different
    priorities for handling contention


## Files included
    Exectuable Files:
    queue.c - bounded buffer queue with push and pop operations, supporting multiple concurrent threads.
    rwlock.c - reader-writer lock with support for different contention strategies (readers-first, writers-first, or N-way)

    Header Files:
    queue.h - Declares the queue API and related structures.
    rwlock.h - Declares the reader-writer lock API and related structures.

    Extras:
    Makefile - Compiles and creates executables
