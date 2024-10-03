#include "path.h"

#include <stdlib.h>

typedef struct path {
    uint32_t total_weight;
    Stack *vertices;
} Path;

Path *path_create(uint32_t capacity) {
    //new path structure
    Path *p = (Path *) malloc(sizeof(Path));

    //initialize stack within path
    p->vertices = stack_create(capacity);

    //sets initial weight to 0
    p->total_weight = 0;

    //return created path
    return p;
}

void path_free(Path **pp) {
    // Check if the pointer to the path and the path itself are not null
    if (pp != NULL && *pp != NULL) {

        // Free the memory associated with the stack within the path
        stack_free(&(*pp)->vertices);

        // Free the memory associated with the path
        free(*pp);
    }

    // Set the pointer to null to avoid double freeing
    if (pp != NULL) {
        *pp = NULL;
    }
}

uint32_t path_vertices(const Path *p) {

    // Check if the path pointer is not null
    if (p != NULL) {

        // Call stack_size on the vertices stack within the path
        return stack_size(p->vertices);
    }

    // If the path pointer is null, return 0
    return 0;
}

uint32_t path_distance(const Path *p) {

    // Check if the path pointer is not null
    if (p != NULL) {

        // Return the total_weight field of the path
        return p->total_weight;
    }

    // If the path pointer is null, return 0
    return 0;
}

void path_add(Path *p, uint32_t val, const Graph *g) {
    uint32_t most_recent_vertex = 0;

    // Check if the path and graph pointers are not null
    if (p != NULL && g != NULL) {
        if (stack_empty(p->vertices)) {

            // If the path is empty, set the total weight to 0
            p->total_weight = 0;
        }

        // If the path is not empty, get the most recent vertex
        stack_peek(p->vertices, &most_recent_vertex);

        // Look up the distance from the last vertex to the new one in the graph
        uint32_t distance = graph_get_weight(g, most_recent_vertex, val);

        // Add the distance to the total weight of the path
        p->total_weight += distance;

        // Add the vertex to the path
        stack_push(p->vertices, val);
    }
}

uint32_t path_remove(Path *p, const Graph *g) {
    uint32_t removed_vertex = 0;

    //cehck if path and graph pointers are not null
    if (p != NULL && g != NULL) {
        if (!stack_empty(p->vertices)) {
            stack_pop(p->vertices, &removed_vertex);
        }

        if (!stack_empty(p->vertices)) {
            uint32_t last_vertex = 0;

            stack_peek(p->vertices, &last_vertex);

            uint32_t distance = graph_get_weight(g, last_vertex, removed_vertex);

            p->total_weight -= distance;
        } else {
            p->total_weight = 0;
        }
    }

    return removed_vertex;
}

void path_copy(Path *dst, const Path *src) {
    if (dst != NULL && src != NULL) {
        //copiew weight
        dst->total_weight = src->total_weight;

        //copies vertices
        stack_copy(dst->vertices, src->vertices);
    }
}

void path_print(const Path *p, FILE *f, const Graph *g) {

    if (p != NULL && f != NULL && g != NULL) {
        fprintf(f, "Alissa starts at:\n");
        fprintf(f, "%s\n", graph_get_vertex_name(g, 0));
        stack_print(p->vertices, f, graph_get_names(g));
        fprintf(f, "Total Distance: %" PRIu32 "\n", p->total_weight);
        return;
    }
}
