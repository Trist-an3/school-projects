## Programming Assignment 7
* Name: Tristan Garcia
* CruzID: tgarcia7
* StudentID: 1991617

## Purpose:
    program reads the frequency of symbols in the input file, builds a Huffman Tree,
    and writes the compressed data to an output file. The decompression program reconstructs 
    the original file using the Huffman Tree.

## Files included
    Exectuable Files:
    huff.c - Implements the Huffman compression algorithm.
    dehuff.c - Implements the Huffman decompression algorithm.

    Source Files:
    bitwriter.c - Handles writing bits to the output file for compression.
    bitreader.c - Handles reading bits from the compressed file for decompression.
    node.c - Implements the binary tree used for the Huffman Tree.
    pq.c - Implements the priority queue for managing the Huffman Tree nodes.

    Testing File:
    prqtest.c - Testing file 

    Helper file:
    bitwriter.h - Declares functions for writing bits.
    bitreader.h - Declares functions for reading bits.
    node.h - Declares functions for the binary tree.
    pq.h - Declares functions for the priority queue.


    Extras:
    Makefile - Compiles and creates executables