#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct graph {
    uint32_t vertices;
    bool directed;
    bool *visited;
    char **names;
    uint32_t **weights;
} Graph;

//Creates and initializes a graph with a specified number of vertices.
Graph *graph_create(uint32_t vertices, bool directed) {
    Graph *g = calloc(1, sizeof(Graph));
    g->vertices = vertices;
    g->directed = directed;
    // use calloc to initialize everything with zeroes
    g->visited = calloc(vertices, sizeof(bool));
    g->names = calloc(vertices, sizeof(char *));
    // allocate g->weights with a pointer for each row
    g->weights = calloc(vertices, sizeof(g->weights[0]));
    // allocate each row in the adjacency matrix

    for (uint32_t i = 0; i < vertices; ++i) {
        g->weights[i] = calloc(vertices, sizeof(g->weights[0][0]));
    }

    return g;
}

//Frees the memory allocated for a graph.
void graph_free(Graph **gp) {
    if (gp != NULL && *gp != NULL) {

        //free mem for adjacency matrix
        for (uint32_t i = 0; i < (*gp)->vertices; i++) {
            free((*gp)->weights[i]);
        }

        free((*gp)->weights);

        //free mem for array of vertex names
        for (uint32_t i = 0; i < (*gp)->vertices; i++) {
            free((*gp)->names[i]);
        }

        free((*gp)->names);

        //free mem for visited array
        free((*gp)->visited);

        free(*gp);

        *gp = NULL;
    }
}

//Returns the number of vertices in the graph.
uint32_t graph_vertices(const Graph *g) {
    if (g == NULL) {
        return 0;
    }

    return g->vertices;
}

//adds an edge between start and end with the weight
void graph_add_edge(Graph *g, uint32_t start, uint32_t end, uint32_t weight) {
    //checks if start and end vertices are valid
    if (start >= g->vertices || end >= g->vertices) {
        return;
    }

    //after check is done it adds edge and weight to the adjacency matrix
    g->weights[start][end] = weight;

    if (!g->directed) {
        g->weights[end][start] = weight;
    }
}

//returns the weight
uint32_t graph_get_weight(const Graph *g, uint32_t start, uint32_t end) {
    //check if vertices are valid
    if (start >= g->vertices || end >= g->vertices) {
        return 0;
    }

    // Return the weight associated with the edge between start and end
    return g->weights[start][end];
}

//Adds the vertex v to the list of visited vertices.
void graph_visit_vertex(Graph *g, uint32_t v) {

    //check if vertex v is valid
    if (v >= g->vertices) {
        return;
    }

    //mark vertex as visited
    g->visited[v] = true;
}

//Removes the vertex v from the list of visited vertices.
void graph_unvisit_vertex(Graph *g, uint32_t v) {

    //check if vertex v is valid
    if (v >= g->vertices) {
        return;
    }

    // Unmark the vertex by setting it to false in the visited array
    g->visited[v] = false;
}

//Returns true if vertex v is visited in graph g, false otherwise.
bool graph_visited(const Graph *g, uint32_t v) {
    if (v >= g->vertices) {
        return false;
    }

    return g->visited[v];
}

char **graph_get_names(const Graph *g) {
    return g->names;
}

void graph_add_vertex(Graph *g, const char *name, uint32_t v) {
    if (g->names[v] != NULL) {
        free(g->names[v]);
    }

    g->names[v] = strdup(name);
}

const char *graph_get_vertex_name(const Graph *g, uint32_t v) {
    if (v >= g->vertices) {
        return NULL;
    }

    return g->names[v];
}

void graph_print(const Graph *g) {
    // Print the number of vertices
    printf("Number of Vertices: %u\n", g->vertices);

    // Print the vertices and their names
    printf("Vertices:\n");
    for (uint32_t i = 0; i < g->vertices; ++i) {
        printf("%u - %s\n", i, g->names[i]);
    }

    // Print the adjacency matrix
    printf("Adjacency Matrix:\n");
    for (uint32_t i = 0; i < g->vertices; ++i) {
        for (uint32_t j = 0; j < g->vertices; ++j) {
            printf("%u ", g->weights[i][j]);
        }
        printf("\n");
    }
}
