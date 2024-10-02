/* Tristan Garcia
 * CSE101 TUES/THURS
 * PA2
 * This is the main file for the graph ADT functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Graph.h"
#include "List.h"

///*** Structs ***/
typedef struct GraphObj {
    List *neighborsList;
    int *colorArray;
    int *parentArray;
    int *distanceArray;
    int order; // # of vertices
    int size; // # of edges
    int source;
} GraphObj;

/*** Constructors-Destructors ***/
Graph newGraph(int n) {

    Graph G = malloc(sizeof(struct GraphObj));
    G -> neighborsList = calloc(n + 1, sizeof(List));
    G -> colorArray = calloc(n + 1, sizeof(int));
    G -> parentArray = calloc(n + 1, sizeof(int));
    G -> distanceArray = calloc(n + 1, sizeof(int));

    for(int i = 0; i <= n; i++) {
        G -> neighborsList[i] = newList();
	G -> parentArray[i] = NIL;
	G -> distanceArray[i] = INF;
    }

    G -> order = n;
    G -> size = 0;
    G -> source = NIL;
	
    return G;
}

void freeGraph(Graph* pG) {
    if(pG != NULL && *pG != NULL) {

        for(int i = 0; i <= (*pG) -> order; i++) {
            freeList(&(*pG) -> neighborsList[i]);
        }

        free((*pG) -> neighborsList);
        free((*pG) -> colorArray);
        free((*pG) -> parentArray);
        free((*pG) -> distanceArray);

        free(*pG);
        *pG = NULL;
    }
}

/*** Access functions ***/
int getOrder(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling getOrder() on NULL graph\n");
        exit(EXIT_FAILURE);
    }

    return G -> order;
}

int getSize(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling getSzie() on NULL graph\n");
        exit(EXIT_FAILURE);
    }

    return G -> size;
}

int getSource(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling getSource() on NULL graph\n");
        exit(EXIT_FAILURE);
    }


    if(G -> source == NIL) {
        return NIL;
    } else {
    return G -> source;  
    }
  	
}

int getParent(Graph G, int u) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling getParent() on NULL graph\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling getDist() with invalid index\n");
        exit(EXIT_FAILURE);
    }

    if(G -> source == NIL) {
        return NIL;
    } 

    return G -> parentArray[u];
}

int getDist(Graph G, int u) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling getDist() on NULL graph\n");
        exit(EXIT_FAILURE);
    }
    
    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling getDist() with invalid index\n");
        exit(EXIT_FAILURE);
    }

    if(G -> source == NIL) {
        return INF;
    } 

    return G -> distanceArray[u];
}

void getPath(List L, Graph G, int u) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling getPath() on NULL graph\n");
        exit(EXIT_FAILURE);
    } else if (L == NULL) {
        fprintf(stderr, "List Error: calling getPath() on NULL list\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling getDist() with invalid index\n");
        exit(EXIT_FAILURE);
    }

   // if(G -> source == NIL) {
    if(getSource(G) == NIL) {
        fprintf(stderr, "POGPOGPOG Graph Error: calling getPath() when source is NIL\n");
        exit(EXIT_FAILURE);
    }
    
    //printpath in pseudocode algorithms tab but can be adapted into getpath
   // if(G -> source == u) {
    if(u == getSource(G)) {
        append(L, u);
    }
    else if(G -> parentArray[u] == NIL) {
        append(L, NIL);
    } else {
        getPath(L, G, G -> parentArray[u]);
        append(L, u);
    }
}

/*** Manipulation procedures ***/
void makeNull(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling makeNull() when graph is NULL\n");
        exit(EXIT_FAILURE);
    }

    int n = getOrder(G);
    freeGraph(&G);
    G = newGraph(n);
}

void addEdge(Graph G, int u, int v) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling addEdge() when graph is NULL\n");
        exit(EXIT_FAILURE);
    }
    
    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling addEdge() with invalid index\n");
        exit(EXIT_FAILURE);
    }

    if(v < 1 || v > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling addEdge() with invalid index\n");
        exit(EXIT_FAILURE);
    }

    addArc(G, u, v);
    addArc(G, v, u);

    G -> size--;
}
// Insert a new directed edge from u to v, maintaining sorted order of adjacency lists.
void addArc(Graph G, int u, int v) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling addArc() when graph is NULL\n");
        exit(EXIT_FAILURE);
    }
    
    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling addArc() with invalid index\n");
            exit(EXIT_FAILURE);
    }

    if(v < 1 || v > getOrder(G)) {
        fprintf(stderr, "Graph Error: calling addArc() with invalid index\n");
        exit(EXIT_FAILURE);
    }

    // If the neighbors list for u is empty, append v to it
    if(length(G->neighborsList[u]) == 0) {
		append(G->neighborsList[u], v);
	    	moveFront(G->neighborsList[u]);
    } else { 
        // If the neighbors list is not empty, traverse it to find the correct position for v

	
	//check if cursor >= to 0 and check value stored at current cursor is less than v
    	while (index(G->neighborsList[u]) >= 0 && get(G->neighborsList[u]) < v) {
        	moveNext(G->neighborsList[u]); 
    	}
 	//insert v correct position 
 	if (index(G->neighborsList[u]) >= 0) {
   		insertBefore(G->neighborsList[u], v);
	} else {
    		append(G->neighborsList[u], v);
	}	
    	//insertBefore(G->neighborsList[u], v);
    }
    G->size++; 
}

void BFS(Graph G, int s) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling BFS() when graph is NULL\n");
        exit(EXIT_FAILURE);
    }

    if(s < 1 || s > getOrder(G)) {
    	fprintf(stderr,"Graph ErroRr: calling BFS() on an source vertex outside the given range\n");
	exit(EXIT_FAILURE);
    }

    //G -> source = s;
    for(int i = 0; i <= getOrder(G); i++){
        G -> colorArray[i] = WHITE;
        G -> distanceArray[i] = INF;
        G -> parentArray[i] = NIL;
    }
    
    G -> source = s;
    //discover source s
    G -> colorArray[s] = GRAY;
    G -> distanceArray[s] = 0;
    G -> parentArray[s] = NIL;

    List L = newList();

    append(L, s);

    while(length(L) > 0 ) {
        int x = front(L);
        deleteFront(L);
        moveFront(G -> neighborsList[x]);

       while(index( G -> neighborsList[x]) != -1) {
            int y = get(G -> neighborsList[x]);	      
            if(G -> colorArray[y] == WHITE) { //y is undiscovered
                G -> colorArray[y] = GRAY;
                G -> distanceArray[y] = G -> distanceArray[x] + 1;
                G -> parentArray[y] = x;
                append(L, y);
            }

            moveNext(G -> neighborsList[x]);
        }

        G -> colorArray[x] = BLACK;
    }
    freeList(&L);
}


/*** Other operations ***/
void printGraph(FILE* out, Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: calling addEdge() when graph is NULL\n");
        exit(EXIT_FAILURE);
    }

    // Iterate over each vertex in the graph 
    for(int i = 1; i <= getOrder(G); i++) {
            fprintf(out, "%d: ", i);

        // Check if the adjacency list for the current vertex is empty
        if(length(G->neighborsList[i]) == 0) {
            fprintf(out, "\n");
        } else {
            moveFront(G->neighborsList[i]);

            // Iterate over the adjacency list and print each neighbor
            while(index(G->neighborsList[i]) >= 0) {
                fprintf(out, "%d", get(G->neighborsList[i]));
                moveNext(G->neighborsList[i]);
		
		if(index(G->neighborsList[i]) >= 0) {
			fprintf(out," ");
		}
            }

            fprintf(out, "\n");
        }
    }
}
