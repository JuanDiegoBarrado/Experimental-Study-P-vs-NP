#include "boolean_functions/boolean_functions.h"
#include <vector>
#include <cstdint>

using ull = uint64_t;

const int NUM_BITS = 5;

truthtable truth_table;
int parse_argument(int argc, char * argv[]);

int main(int argc, char * argv[]){
    // ── Argument parsing ─────────────────────────────────────────────────────
    if (parse_argument(argc, argv) != 0)
        return -1;
    
    cout << "Function: " << print_table(truth_table) << "\n";

    vector<minbool::MinTerm<NUM_BITS>> resp = minimize<NUM_BITS>(truth_table);
    for (minbool::MinTerm<NUM_BITS>& v : resp) {
        for (int i = 0; i < NUM_BITS; i++) {
            if (v[i] == minbool::MinTerm<NUM_BITS>::Dash)
                continue;
            else if (v[i] == minbool::MinTerm<NUM_BITS>::Zero)
                cout << "¬";
            cout << "x" << i+1 << " ";
        }
        cout << "\n";
    }

    return 0;
}

int parse_argument(int argc, char * argv[]) {
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-i" || arg == "--int") && i + 1 < argc) {
            truth_table = to_truthtable(stoull(argv[++i]), 32);
        } else if ((arg == "-s" || arg == "--string") && i + 1 < argc) {
            truth_table = to_truthtable(argv[++i], 32);
        } else {
            cerr << "Unknown argument: " << arg << "\n";
            return -1;
        }
    }
    return 0;
}