#pragma once

#include <vector>
#include <iostream>
#include <cstdint>
#include <bitset>
#include <cassert>
#include <memory>

#include "minbool/minbool.h"

using namespace std;
using ui32 = uint32_t;
using ui64 = uint64_t;

const int MAX_INPUT_BITS = 32;
using chain = bitset<MAX_INPUT_BITS>;
using truthtable = vector<bool>;

template<typename Func>
truthtable compute_truth_table(Func func, int n_bits, ui32 print_interval = 0) {
    assert(n_bits <= MAX_INPUT_BITS);

    ui64 size = ui64(1) << n_bits;
    truthtable output(size, false);
    
    for (ui64 i = 0; i < size; i++) {
        chain input(i);
        output[i] = func(input, n_bits);

        if (print_interval != 0 && i % print_interval == 0 && i != 0)
            cout << "\rLlevamos " << i / print_interval << " iteraciones de " << size / print_interval;
    }

    return output;
}

template <size_t Nbits>
vector<minbool::MinTerm<Nbits>> compute_minimal_DNF(const truthtable& table) {
    using IntTypeN = typename minbool::MinTerm<Nbits>::IntTypeN;
    vector<IntTypeN> on_values, dc_values;

    for (uint32_t i = 0; i < table.size(); ++i)
        if (table[i])
            on_values.push_back((IntTypeN)i);  // cast aquí

    vector<minbool::MinTerm<Nbits>> solution = minbool::minimize_boolean<Nbits>(on_values, dc_values);
    return solution;
}

template <size_t Nbits>
vector<minbool::MinTerm<Nbits>> compute_minimal_CNF(const truthtable& table) {
    using IntTypeN = typename minbool::MinTerm<Nbits>::IntTypeN;
    vector<IntTypeN> on_values, dc_values;

    for (uint32_t i = 0; i < table.size(); ++i)
        if (!table[i])
            on_values.push_back((IntTypeN)i);  // cast aquí

    vector<minbool::MinTerm<Nbits>> solution = minbool::minimize_boolean<Nbits>(on_values, dc_values);
    return solution;
}

string print_table(truthtable& t);
truthtable to_truthtable(uint32_t n, int bits = 32);
truthtable to_truthtable(string n, int bits);

bool identity_function(const chain& c, int n_bits, int bit);
bool parity_function(const chain& c, int n_bits);
bool majority_function(const chain& c, int n_bits);
bool and_function(const chain& c, int n_bits);
bool or_function(const chain& c, int n_bits);
bool clique_function(const chain& c, int n_bits);
bool enumerate_function(const chain& c, int n_bits);
bool is_prime_naive_function(const chain& c, int n_bits);