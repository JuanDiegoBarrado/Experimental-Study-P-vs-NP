#pragma once

#include <bitset>
#include <vector>
#include <iostream>
#include <cstdint>
#include <bitset>
#include <cassert>

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
    chain input;
    truthtable output(size, false);
    
    for (ui64 i = 0; i < size; i++) {
        input = chain(i);
        output[i] = func(input, n_bits);

        if (print_interval != 0 && i % print_interval == 0 && i != 0)
            cout << "\rLlevamos " << i / print_interval << " iteraciones de " << size / print_interval;
    }

    return output;
}

bool identity_function(const chain& c, int n_bits, int bit);
bool parity_function(const chain& c, int n_bits);
bool and_function(const chain& c, int n_bits);
bool or_function(const chain& c, int n_bits);
bool clique_function(const chain& c, int n_bits);
bool enumerate_function(const chain& c, int n_bits);
bool is_prime_naive_function(const chain& c, int n_bits);