/*
 * Tristan Garcia
 * CSE101 TUES/THURS
 * PA2
 * This is the main program file kekw
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"
#include "Graph.h"

int main(int argc, char **argv) {
    int numVertices = 0;
    int vertex1 = 0;
    int vertex2 = 0;

    FILE *infile;
    FILE *outfile;
    
    //checks if correct # of arguements are in
    if(argc != 3) {
        fprintf(stderr, "Error: Invalid number of arguements, need 3\n");
        exit(EXIT_FAILURE);
    }
    
    //Open input and ouput files
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");
    
    //if any of hte files are bad then error
    if(infile == NULL || outfile == NULL) {
        fprintf(stderr, "Error: File can't be opened or read\n");
        exit(EXIT_FAILURE);
    }

    //read # of vertices from input file and creatre new graph
    fscanf(infile, "%d", &numVertices);
    Graph G = newGraph(numVertices);

    //read edges from input file and add them to graph
    while (fscanf(infile, "%d %d", &vertex1, &vertex2) == 2) {
        if (vertex1 == 0 && vertex2 == 0) {
            break;
	}
	addEdge(G, vertex1, vertex2);
    }	
    
    //print adj list representation of graph	
    printGraph(outfile, G);

    while (fscanf(infile, "%d %d", &vertex1, &vertex2) == 2) {
	if (vertex1 == 0 || vertex2 == 0) {
    		break;
	}
        
        //run bfs algo on graph with sorce vertex1 & 2
        BFS(G, vertex1);
	int distance = getDist(G, vertex2);
        List L = newList();

	//output info based on dsitance
        fprintf(outfile, "\nThe distance from %d to %d is ", vertex1, vertex2);

        if (distance != INF) {
            fprintf(outfile, "%d\n", distance);

            if (distance == 0) {
                fprintf(outfile, "A shortest %d-%d path is: %d\n", vertex1, vertex2, vertex1);
            } else {
		//get and print shortest path
                getPath(L, G, vertex2);
                fprintf(outfile, "A shortest %d-%d path is: ", vertex1, vertex2);
                printList(outfile, L);
                fprintf(outfile, "\n");
            }
        } else {
            fprintf(outfile, "infinity\n");
            fprintf(outfile, "No %d-%d path exists\n", vertex1, vertex2);
        }
	//free list mem
    	freeList(&L);
    }
    //free graph mem and close files
    freeGraph(&G);
    fclose(infile);
    fclose(outfile);

    return 0;
}
