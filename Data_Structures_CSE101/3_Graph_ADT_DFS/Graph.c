/*******************************************************
* Name: Tristan Garcia
* CruzID: Tgarcia7
* StudentID: 1991617
* File: Graph.c
* ADT file for the Graph Functions
*******************************************************/

//#include <stdio.h>
#include "Graph.h"

/*********** Structs ***********/

typedef struct GraphObj {
    List *neighbors;
    int *color;
    int *parent;
    int *discover;
    int *finish;
    int order;
    int size;
} GraphObj;


/*********** Constructors Destructors ***********/
Graph newGraph(int n) {
    Graph G = malloc(sizeof(GraphObj));
    G -> neighbors = calloc(n + 1, sizeof(List));
    G -> color = calloc(n + 1, sizeof(int));
    G -> parent = calloc(n + 1, sizeof(int));
    G -> discover = calloc(n + 1, sizeof(int));
    G -> finish = calloc(n + 1, sizeof(int));
    
    for(int i = 1; i <= n; i++) {
        G -> neighbors[i] = newList();
        G -> parent[i] = NIL;
        G -> discover[i] = UNDEF;
        G -> finish[i] = UNDEF;
    }

    G -> order = n;
    G -> size = 0;

    return G;
}

void freeGraph(Graph* pG) {
    if(*pG != NULL && pG != NULL) {

        for(int i = 1; i <= getOrder(*pG); i++) {
            freeList(&(*pG) -> neighbors[i]);
        }

        free((*pG) -> neighbors);
	free((*pG) -> color);
        free((*pG) -> parent);
        free((*pG) -> discover);
        free((*pG) -> finish);

        free(*pG);
        *pG = NULL;
    }
}

/*********** Access Functions ***********/
int getOrder(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling getOrder() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    return G -> order;
}

int getSize(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling getSize() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    return G -> size;
}

int getParent(Graph G, int u) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling getParent() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling getParent() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    return G -> parent[u];
} 

int getDiscover(Graph G, int u) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling getDiscover() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling getDiscover() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(G -> discover[u] == UNDEF) {
        return UNDEF;
    } else {
        return G -> discover[u];
    }

}

int getFinish(Graph G, int u) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling getFinish() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling getFinish() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }
    
    if(G -> finish[u] == UNDEF) {
        return UNDEF;
    } else {
        return G -> finish[u];
    }

}

/*********** Manipulation procedures ***********/
void addArc(Graph G, int u, int v) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling addArc() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling addArc() on invalid vertex.\n");
        exit(EXIT_FAILURE);
    }


    if(v < 1 || v > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling addArc() on invalid vertex.\n");
        exit(EXIT_FAILURE);
    }
    
    
    if(length(G -> neighbors[u]) == 0) {
        append(G -> neighbors[u], v);
        moveFront(G -> neighbors[u]);
        G -> size++;
    } else {
        moveFront(G -> neighbors[u]);
        while(index(G -> neighbors[u]) != -1 && get(G -> neighbors[u]) < v) {
            moveNext(G -> neighbors[u]);
        }

        if (index(G->neighbors[u]) != -1 && get(G->neighbors[u]) == v) {
            return;
    	}

        if(index(G -> neighbors[u]) != -1) {
            insertBefore(G -> neighbors[u], v);
            G -> size++;
        } else {
            append(G -> neighbors[u], v);
            G -> size++;
        }
    }
}

void addEdge(Graph G, int u, int v) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling addEdge() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(u < 1 || u > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling addEdge() on invalid vertex.\n");
        exit(EXIT_FAILURE);
    }


    if(v < 1 || v > getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling addEdge() on invalid vertex.\n");
        exit(EXIT_FAILURE);
    }

    addArc(G, u, v);
    addArc(G, v, u);

    G -> size--;
} 

void visit(Graph G, List S, int u, int *time) {
    int v;
    G -> discover[u] = (++(*time));
    G -> color[u] = GRAY;

    for(moveFront(G -> neighbors[u]); index(G -> neighbors[u]) != -1; moveNext(G -> neighbors[u])) {
        v = get(G -> neighbors[u]);
        if(G -> color[v] == WHITE) {
            G -> parent[v] = u;
            visit(G, S, v, time);
        }
    }

    G -> color[u] = BLACK;
    G -> finish[u] = (++(*time));

    prepend(S, u);
}

void DFS(Graph G, List S) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling DFS() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    if(S == NULL) {
        fprintf(stderr, "Graph Error: Calling DFS() on NULL List reference.\n");
        exit(EXIT_FAILURE);
    } 

    if(length(S) != getOrder(G)) {
        fprintf(stderr, "Graph Error: Calling DFS() when S is not equal to graph size.\n");
        exit(EXIT_FAILURE);
    }
    
    List L = copyList(S);
    clear(S);

    for(moveFront(L); index(L) != -1; moveNext(L)) {
        int i = get(L);
        G -> color[i] = WHITE;
        G -> parent[i] = NIL;
    }

    int time = 0;
    for(moveFront(L); index(L) != -1; moveNext(L)) {
   	int i = get(L);
        if(G -> color[i] == WHITE) {
            visit(G, S, i, &time);
        }
    }
    freeList(&L);
}

/*********** Other Functions ***********/
Graph transpose(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling DFS() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    Graph GT = newGraph(getOrder(G));
    for(int i = 1; i <= getOrder(G); i++) {
        moveFront(G -> neighbors[i]);
        while(index(G -> neighbors[i]) != -1) {
            int temp = get(G -> neighbors[i]);
            addArc(GT, temp, i);
            moveNext(G -> neighbors[i]);
        }
    }

    return GT;
}

Graph copyGraph(Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling copyGraph() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    Graph CG = newGraph(getOrder(G));
    for(int i = 1; i <= getOrder(G); i++) {
        for(moveFront(G -> neighbors[i]); index(G -> neighbors[i]) != -1; moveNext(G -> neighbors[i])) {
            int v = get(G -> neighbors[i]);

            addArc(CG, i, v);
        }
    }

    return CG;
}

void printGraph(FILE* out , Graph G) {
    if(G == NULL) {
        fprintf(stderr, "Graph Error: Calling printGraph() on NULL Graph reference.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i <= getOrder(G); i++) {
        fprintf(out, "%d: ", i);
        printList(out, G -> neighbors[i]);
        fprintf(out, "\n");
    }
}

