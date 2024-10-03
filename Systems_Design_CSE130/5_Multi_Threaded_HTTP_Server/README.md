## Programming Assignment 5
* Name: Tristan Garcia
* CruzID: tgarcia7
* StudentID: 1991617

## Purpose:
    multi-threaded HTTP server in C that can handle multiple client requests simultaneously.
    Building upon the previous HTTP server and concurrent data structures assignments,
    this server utilizes a thread-safe queue and a pool of worker threads to efficiently 
    manage incoming connections and requests.

## Files included
    Exectuable Files:
    httpserver.c: Implements the multi-threaded HTTP server, including request handling, 
    synchronization using pthreads, and generating an audit log.

    Header Files:
    queue.h - Declares the queue API used for managing the request queue.
    rwlock.h - Declares the reader-writer lock API for controlling access to shared resources.
    debug.h - Provides debugging macros and utilities for tracking the server's behavior during execution.
    protocol.h - Declares the HTTP protocol functions, including request parsing and response formatting.

    Extras:
    Makefile - Compiles and creates executables
    Used rwlock and queue files from previous assignment
    Used httpserver file from previous assignment and built on it