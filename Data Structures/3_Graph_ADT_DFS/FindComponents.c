/*******************************************************
* Name: Tristan Garcia
* CruzID: Tgarcia7
* StudentID: 1991617
* File: FindingComponents.c
* Main file for running DFS
********************************************************/

#include "Graph.h"

int main(int argc, char **argv) {
    int n;
    int u;
    int v;
    int components = 0;
    int strongComponents = 0;


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

    fscanf(infile, "%d", &n);
    Graph G = newGraph(n);
 
    do {
        fscanf(infile, "%d", &u);
        fscanf(infile, "%d", &v);
        if(u != 0 && v != 0) {
            addArc(G, u, v);
        }
    } while(v != 0 && u != 0);

    fprintf(outfile, "Adjacency list representation of G:\n");
    printGraph(outfile, G);
    fprintf(outfile, "\n");
	
    Graph reverse = transpose(G);
    List stack = newList();
    List componentList = newList();
    
    int i = 1;
    do {
        append(stack, i);
        i++;
    } while (i <= getOrder(G));   
 
    DFS(G, stack);
    DFS(reverse, stack);

    moveFront(stack);
    while(index(stack) != -1){
    	int temp = get(stack);
	if(getParent(reverse, temp) == NIL){
	    strongComponents++;
	}
	moveNext(stack);
    }

    fprintf(outfile, "G contains %d strongly connected components:\n", strongComponents);
    moveBack(stack);
    while (index(stack) != -1) {
    	int temp = get(stack);	
        if(getParent(reverse, temp) != NIL){
		prepend(componentList, temp);
	} else {
		prepend(componentList, temp);
		components++;
		fprintf(outfile,"Component %d: ", components);
		printList(outfile, componentList);
		fprintf(outfile,"\n");
		clear(componentList);
	}
	movePrev(stack);
    }


    freeGraph(&G);
    freeGraph(&reverse);

    freeList(&stack);
    freeList(&componentList);

    fclose(infile);
    fclose(outfile);

    return 0;
}


