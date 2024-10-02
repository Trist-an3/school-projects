/*********************************************************
* Name: Tristan Garcia
* CruzID: Tgarcia7
* StudentID: 1991617
* File: Graph.h
* Header file for the Graph ADT with function prototypes
*********************************************************/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "List.h"

#define NIL -1
#define UNDEF -2
#define WHITE 1
#define GRAY 2
#define BLACK 3

typedef struct GraphObj *Graph;

/*********** Constructors Destructors ***********/
//Function newGraph() returns a Graph pointing to a newly created GraphObj
Graph newGraph(int n);

//Function freeGraph() frees all heap memory associated with the Graph *pG, set to NULL
void freeGraph(Graph* pG);

/*********** Access Functions ***********/
// Return the number of vertices in the graph.
int getOrder(Graph G);

// Return the number of edges in the graph.
int getSize(Graph G);

// Return the parent of vertex u in the DFS tree, or NIL if DFS has not yet been called.
// Pre: 1<=u<=n=getOrder(G)
int getParent(Graph G, int u); 

// Return discover time of node
// Pre: 1<=u<=n=getOrder(G)
// UNDEF b4 DFS called
int getDiscover(Graph G, int u);

// Return the finish time of node
// Pre: 1<=u<=n=getOrder(G)
// UNDEF b4 DFS called
int getFinish(Graph G, int u); 

/*********** Manipulation procedures ***********/
// Insert new directed edge from u to v
// Pre: 1<=u<=n, 1<=v<=n
void addArc(Graph G, int u, int v); 

// Insert new undirected edge from u to v
// Pre: 1<=u<=n, 1<=v<=n
void addEdge(Graph G, int u, int v); 

// Runs DFS algo on Graph G using List S as stack
// Pre: length(S)==getOrder(G)
void DFS(Graph G, List S); 

/*********** Other Functions ***********/
// Returns a new graph representing the transpose (reverse of edges) of the input graph G.
Graph transpose(Graph G);

// returns a copy of the Graph to a new Graph
Graph copyGraph(Graph G);

// Prints the adj list representation of G to the outfile
void printGraph(FILE* out , Graph G);


