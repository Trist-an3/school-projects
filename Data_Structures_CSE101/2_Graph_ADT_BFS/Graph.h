/* Tristan Garcia
 * CSE101 TUES/THURS
 * PA2
 * This is the header file for the graph ADT functions
*/



#ifndef GRAPH_H_INCLUDE
#define GRAPH_H_INCLUDE

#include "List.h"

#define INF -17
#define NIL -12

#define WHITE 1
#define GRAY 2
#define BLACK 3

typedef struct GraphObj* Graph;

/*** Constructors-Destructors ***/

//Function newGraph() returns a Graph pointing to a newly created GraphObj
//representing a graph having n vertices and no edges

Graph newGraph(int n); 

//Function freeGraph() frees all heap memory associated with the Graph *pG,
//then sets the handle *pG to NULL

void freeGraph(Graph* pG);

/*** Access functions ***/

// Get the number of vertices in the graph.
int getOrder(Graph G);

// Get the number of edges in the graph.
int getSize(Graph G);

// Get the source vertex most recently used in BFS, or NIL if BFS has not yet been called.
int getSource(Graph G);

// Get the parent of vertex u in the BFS tree, or NIL if BFS has not yet been called.
// Preconditions: 1 ≤ u ≤ getOrder(G).
int getParent(Graph G, int u);

// Get the distance from the most recent BFS source to vertex u, or INF if BFS has not yet been called.
// Preconditions: 1 ≤ u ≤ getOrder(G).
int getDist(Graph G, int u);

// Append to the list L the vertices of a shortest path from source to u, or append NIL if no such path exists.
// Preconditions: getSource(G) != NIL (BFS must be called before getPath is called).
// Preconditions: 1 ≤ u ≤ getOrder(G).
void getPath(List L, Graph G, int u);

/*** Manipulation procedures ***/

// Delete all edges of G, restoring it to its original state (null graph).
void makeNull(Graph G);

// Insert a new edge joining u to v, maintaining sorted order of adjacency lists.
// Preconditions: 1 ≤ u, v ≤ getOrder(G).
void addEdge(Graph G, int u, int v);

// Insert a new directed edge from u to v, maintaining sorted order of adjacency lists.
// Preconditions: 1 ≤ u, v ≤ getOrder(G).
void addArc(Graph G, int u, int v);

// Run the BFS algorithm on the graph G with source s, updating color, distance, parent, and source fields.
void BFS(Graph G, int s);

/*** Other operations ***/

// Print the adjacency list representation of G to the file pointed to by out.
void printGraph(FILE* out, Graph G);

#endif

