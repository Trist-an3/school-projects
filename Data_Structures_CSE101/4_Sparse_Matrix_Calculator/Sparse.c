/********************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA4
* File: Sparse.c
* Main file 
*********************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#include"List.h"
#include"Matrix.h"


int main(int argc, char** argv){

  FILE *infile; 
  FILE *outfile; 


    if(argc != 3) {
        fprintf(stderr, "Error: Invalid number of arguements, need 3.\n");
        exit(EXIT_FAILURE);
    }

    
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");


    if(infile == NULL || outfile == NULL) {
        fprintf(stderr,"Error: Unable to open file(s) for operation.\n");
        exit(EXIT_FAILURE);
    }

    fclose(infile);
    fclose(outfile);

    return 0;
}


