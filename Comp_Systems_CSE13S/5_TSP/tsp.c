/* 
*Tristan Garcia
*CSE13s
*Surfin USA TSP problem
*This program reads graphs representing cities from the input file
*it uses dfs to find the most optimal path for Alissa to visit 
*each city exactly once  and return to the start
*/

#include "graph.h"
#include "path.h"
#include "stack.h"
#include "vertices.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OPTIONS "i:o:dh"

// Counter to track the number of valid paths found
static int valid_path_count = 0;

void dfs(uint32_t node, Graph *g, Path *current_path, Path *best_path, char *vertices[]) {

    // Visit the current vertex in the graph
    graph_visit_vertex(g, node);

    // Check if the current path is a valid solution
    if ((graph_get_weight(g, node, START_VERTEX) != 0)
        && (path_vertices(current_path) == graph_vertices(g) - 1)) {

        // Add the start vertex to close the path
        path_add(current_path, START_VERTEX, g);

        // Check if this path is the first valid one or has a shorter distance than the best path
        if (valid_path_count == 0 || path_distance(current_path) < path_distance(best_path)) {

            // Copy the current path as the best path and mark it as the first valid path
            path_copy(best_path, current_path);

            // Increment the count of valid paths found during DFS
            valid_path_count++;
        }

        //Remove the start vertex to backtrack
        path_remove(current_path, g);
    }

    // look at other vertices
    for (uint32_t i = 0; i < graph_vertices(g); i++) {

        //Check if there is a valid edge to an unvisited vertex
        if (graph_get_weight(g, node, i) > 0 && !graph_visited(g, i)) {

            //visit vertex and add it to the path
            graph_visit_vertex(g, i);
            path_add(current_path, i, g);

            //call dfs to explore path
            dfs(i, g, current_path, best_path, vertices);

            // Unvisit the vertex and remove it from the current path to backtrack
            graph_unvisit_vertex(g, i);
            path_remove(current_path, g);
        }
    }
}

int main(int argc, char **argv) {
    int opt = 0;

    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t weight = 0;
    uint32_t total_vertices = 0;
    uint32_t total_edges = 0;

    bool option_h = false;
    bool option_d = false;

    FILE *infile = stdin;
    FILE *outfile = stdout;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                fprintf(stderr, "Error opening input file %s\n", optarg);
                option_h = true;
            }
            break;
        case 'o': outfile = fopen(optarg, "w"); break;

        case 'd': option_d = true; break;
        case 'h': option_h = true; break;
        default:
            fprintf(stderr, "tsp: unknown or poorly formatted option '%c'\n", optopt);
            return 1;
        }
    }

    if (option_h) {
        printf("Usage: tsp [options]\n\n");
        printf("-i infile    Specify the input file path containing the cities and edges\n");
        printf("             of a graph. If not specified, the default input should be\n");
        printf("             set as stdin.\n\n");
        printf("-o outfile   Specify the output file path to print to. If not specified,\n");
        printf("             the default output should be set as stdout.\n\n");
        printf("-d           Specifies the graph to be directed.\n\n");
        printf("-h           Prints out a help message describing the purpose of the\n");
        printf("             graph and the command-line options it accepts, exiting the\n");
        printf("             program afterwards.\n");
        return 0;
    }

    //reads total number of verticies from input file
    fscanf(infile, "%u\n", &total_vertices);

    //create paths and graphs
    Graph *g = graph_create(total_vertices, option_d);
    Path *current_path = path_create(total_vertices);
    Path *best_path = path_create(total_vertices);

    //allocate some memory for vertex name
    char *name = calloc(20, sizeof(char *));

    // Allocate memory for an array of city names to store the names of vertices in the graph.
    char **vertex_names = (char **) calloc(total_vertices, sizeof(char *));

    //makes sure user puts a number for # of vertices and if not it returns an error
    if (total_vertices < 1) {
        fprintf(stderr, "tsp:  error reading number of vertices\n");

        //free everything b4 exiting program
        free(vertex_names);
        free(name);
        graph_free(&g);
        path_free(&current_path);
        path_free(&best_path);
        return 1;
    }

    //Iterate over each vertex to read its name from the input file and add it to the graph
    for (uint32_t i = 0; i < total_vertices; i++) {

        //reads name of vertex from the input file
        fgets(name, 1024, infile);

        //removes the new line char at the end of name
        name[strlen(name) - 1] = '\0';

        //adds vertex to graph with name and its index
        graph_add_vertex(g, name, i);

        //stores a duplicate of the name array to call at end
        vertex_names[i] = strdup(name);

        //     printf("Added vertex: %s with index: %u\n", name, i);
    }

    //Read the total number of edges from the input file then value stored in total edges
    fscanf(infile, "%u\n", &total_edges);
    for (uint32_t i = 0; i < total_edges; i++) {

        // Read the start vertex, end vertex, and weight of an edge from the input file
        fscanf(infile,
            "%u"
            "%u"
            "%u",
            &start, &end, &weight);

        //adds edge to the graph
        graph_add_edge(g, start, end, weight);
        //printf("src edge is %u, dst edge is %u, weight: %u\n",start,end, weight);
    }

    //does search starting from the very first vertex then finds the most optimal path
    dfs(START_VERTEX, g, current_path, best_path, vertex_names);

    //Check if the best path has a distance of 0, indicating no valid path was found.
    if (path_distance(best_path) == 0) {
        fprintf(outfile, "No path found! Alissa is lost!\n");

        //freeing the each vertex[i] in the array
        for (uint32_t i = 0; i < total_vertices; i += 1) {
            free(vertex_names[i]);
        }

        //free everything else b.c program ends
        free(vertex_names);
        free(name);
        graph_free(&g);
        path_free(&current_path);
        path_free(&best_path);
        return 0;
    } else {
        //prints out the result
        path_print(best_path, outfile, g);
    }

    /***************************************************************************************/
    //Free everything and program ends

    for (uint32_t i = 0; i < total_vertices; i += 1) {
        free(vertex_names[i]);
    }

    free(vertex_names);

    free(name);
    graph_free(&g);
    path_free(&current_path);
    path_free(&best_path);

    fclose(outfile);
    fclose(infile);
    return 0;
}
