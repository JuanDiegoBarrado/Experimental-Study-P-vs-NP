#include <bits/stdc++.h>
#include "graph.h"
using namespace std;

#define MAX_BITS 32
using chain = bitset<MAX_BITS>;
using ull = unsigned long long;


void print_chain(const chain& c, int n_bits = 0) {
    chain _c = c;
    int i = 0;
    if (n_bits > 0) {
        for (int i = 0; i < n_bits; i++)
            cout << _c[i];
    }
    else {
        for (int i = 0; i < MAX_BITS && !_c.any(); i++) {
            cout << _c[i];
            _c.reset(i);
            i++;
        }
    }
}

ull convert_bits_to_int(const chain& c, int n_bits = MAX_BITS) {
    ull pot = 1;
    ull val = 0;
    for (int i = 0; i < n_bits; i++) {
        val += c[i] * pot;
        pot *= 2;
    }

    return val;
}

template<typename function>
void compute_truth_table(function func, vector<bool>& output, int n_bits) {
    ull size = 1 << n_bits;
    chain input;
    ull next_print = 150000;

    for (int i = 0; i < size; i++) {
        if (i == next_print) {
            next_print = i + next_print;
            cout << "Llevamos " << i << " iteraciones...\n";
        }

        input = chain(i);
        output[i] = func(input, n_bits);
    }
}

bool parity_function(const chain& c, int n_bits) {
    return c.count() % 2 == 0;
}

bool and_function(const chain& c, int n_bits) {
    return c.all();
}

bool or_function(const chain& c, int n_bits) {
    return c.any();
}

bool nand_function(const chain& c, int n_bits, int i, int j) {
    return !(c[i] && c[j]);
}

bool clique_function(const chain& c, int n_bits) {
    ull v = (1 + sqrt(1 + 8*n_bits)) / 2;
    vector<bool> graph;
    for (int i = 0; i < n_bits; i++)
        graph.push_back(c[i]);
    vector<vector<bool>> matrix = decode_graph(graph, v, n_bits);
    Maxclique clique_calculator = Maxclique(matrix);
    vi max_clique = clique_calculator.maxClique();
    return (max_clique.size() >= (v / 2));
}

// TODO
bool enumerate_function(const chain& c, int n_bits) {
    ull table_line = convert_bits_to_int(c, n_bits);
    ull subgrup = table_line / 8;
    ull index = table_line % 8;
    chain num(subgrup);
    
    return num[7-index];
}

bool is_prime_naive(const chain& c, int n_bits) {
    ull n = convert_bits_to_int(c, n_bits);
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