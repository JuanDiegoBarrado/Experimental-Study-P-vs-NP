#include <math.h>
#include "boolean_functions/boolean_functions.h"
#include "graph_manipulations/graph.h"
using namespace std;

string print_table(truthtable& t) {
    std::string s;
    s.resize(t.size());
    for (int i = 0; i < t.size(); i++)
        s[t.size()-1 - i] = (t[i]) ? '1' : '0'; // Invierte al imprimir 
    return s;
}

truthtable to_truthtable(uint32_t n, int bits) {
    std::vector<bool> v(bits);
    for (int i = 0; i < bits; ++i)
        v[i] = (n >> i) & 1;
    return v;
}

truthtable to_truthtable(string n, int bits) {
    std::vector<bool> v(bits);
    for (int i = 0; i < bits; ++i)
        v[i] = (n[bits-1 - i] == '1');
    return v;
}

bool identity_function(const chain &c, int n_bits, int bit) {
    return c[bit]; // TODO check is a proper bit
}

bool parity_function(const chain& c, int n_bits) {
    return c.count() % 2 == 0;
}

bool majority_function(const chain &c, int n_bits) {
    return c.count() > n_bits / 2;
}

bool and_function(const chain& c, int n_bits) {
    return c.all();
}

bool or_function(const chain& c, int n_bits) {
    return c.any();
}

bool clique_function(const chain& c, int n_bits) {
    vector<vector<bool>> matrix = decode_graph(c, n_bits);
    Maxclique clique_calculator = Maxclique(matrix);
    vi max_clique = clique_calculator.maxClique();
    return (max_clique.size() >= (matrix.size() / 2));
}

// TODO
bool enumerate_function(const chain& c, int n_bits) {
    ui32 table_line = c.to_ulong();
    ui32 subgrup = table_line / 8;
    ui32 index = table_line % 8;
    chain num(subgrup);
    
    return num[7-index];
}

bool is_prime_naive_function(const chain& c, int n_bits) {
    ui32 n = c.to_ulong();
    if (n == 0 || n == 1)
        return false;
    else if (n == 2)
        return true;
    
    for (int i = 2; i < sqrt(n)+1; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}