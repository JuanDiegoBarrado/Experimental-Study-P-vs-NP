#include <vector>
#include <iomanip>
#include <algorithm>
#include "boolean_functions/boolean_functions.h"

using namespace std;

const int NUM_BITS = 21;
const ui64 TABLE_LENGTH = 1 << (NUM_BITS-1);

int main() {
    auto func = [](const chain& c, int n_bits) {
        return parity_function(c, n_bits);
    };
    
    truthtable truth_table = compute_truth_table(func, NUM_BITS, 1000000);
    string truth_table_str; truth_table_str.resize(truth_table.size());

    for (ui64 i = 0; i < truth_table.size(); i++)
        truth_table_str[i] = truth_table[i] ? '1' : '0';

    reverse(truth_table_str.begin(), truth_table_str.end());

    cout << truth_table_str << endl;

    return 0;
}