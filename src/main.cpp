#include <bits/stdc++.h>
#include "boolean_functions.h"

using namespace std;

int main() {
    int N_BITS = 5;
    int f_length = 1;
    for (int i = 0; i < N_BITS; i++)
        f_length *= 2;

    vector<bool> f(f_length);
    cout << "Total: " << f_length << "\n";

    auto func = [](const chain& c, int n_bits) {
        return enumerate_function(c, n_bits);
    };
    compute_truth_table(func, f, N_BITS);
    for (bool b : f)
        cout << b;
    cout << "\n";
    
        unsigned long value = 0;
    for (int i = 0; i < 32; i++)
        value = (value << 1) | (f[i] ? 1 : 0);
    cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << value;

    return 0;
}