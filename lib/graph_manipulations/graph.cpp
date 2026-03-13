#include <math.h>
#include "graph_manipulations/graph.h"
#include "boolean_functions/boolean_functions.h"

vvb decode_graph(const chain& graph_codification, int n_bits) {
    int num_edges = n_bits; // TODO it has to be checked it is a proper number
    int num_vertexs = (1 + sqrt(1 + 8*num_edges)) / 2;
    vvb graph_matrix(num_vertexs, vb(num_vertexs, false));

    int k = 0;
    for (int i = 1; i < num_vertexs; i++) {
        for (int j = 0; j < i; j++) {
            graph_matrix[i][j] = graph_codification[k];
            graph_matrix[j][i] = graph_codification[k];
            k++;
        }
    }

    return graph_matrix;
}