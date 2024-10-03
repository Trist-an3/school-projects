/* Tristan Garcia 
 * CSE101 TUES/THURS
 * PA1
 * This is main program file for this project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "List.h"

#define MAX_LEN 400

int main(int argc, char **argv) {
    int lines = 0;
    char buffer[MAX_LEN];

    FILE *in_file;
    FILE *out_file; 

    //Checks if user puts correct value of arguements
    if(argc != 3) {
        fprintf(stderr,"Error: invalid number of arguements, need 3\n");
        printf("Usage: ./lex <inputfile> <outputfile>\n");
        exit(EXIT_FAILURE);
    }
   
    //responsible for opening, reading and writing functions
    in_file = fopen(argv[1], "r");
    out_file = fopen(argv[2], "w");

    //if input or output file is NULL then error
    if(in_file == NULL || out_file == NULL) {
        fprintf(stderr, "Error: Input or Output file is NULL\n");
        exit(EXIT_FAILURE);
    }

    //Count lines of the input file
    while(fgets(buffer, MAX_LEN, in_file) != NULL) {
        lines++;
    }
    
    //reset in_file to beginning
    rewind(in_file);

	
    //allocating memory for strings array
    char** linesArray = (char**)calloc(lines, sizeof(char*));
  

    fclose(in_file);
    fclose(out_file);


}   
