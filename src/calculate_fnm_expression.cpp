#include "boolean_functions/boolean_functions.h"
#include "metrics/metrics.h"
#include <vector>
#include <cstdint>
using ull = unsigned long long int;

const int NUM_BITS = 5;
truthtable truth_table;

int parse_argument(int argc, char * argv[]);

int main(int argc, char * argv[]){
    // ── Argument parsing ─────────────────────────────────────────────────────
    if (parse_argument(argc, argv) != 0)
        return -1;
    
    cout << "Function: " << print_table(truth_table) << "\n";

    auto exp = fnm<NUM_BITS>(truth_table);
    string line = exp->to_string();
    string tree = exp->to_tree_string();
    ull score = distinct_fnms<NUM_BITS>(truth_table);

    cout << "FNM: " << line << "\n";
    cout << "Score: " << score << "\n";
    cout << "FNM tree:\n" << tree << "\n";

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