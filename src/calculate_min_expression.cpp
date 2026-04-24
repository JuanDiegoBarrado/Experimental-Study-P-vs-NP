#include "boolean_functions/boolean_functions.h"
#include <vector>
#include <cstdint>
#include <cstring>

using ull = uint64_t;

const int NUM_BITS = 5;

bool isDNF;
truthtable truth_table;

void print_usage(const char* prog);
int parse_argument(int argc, char * argv[]);

int main(int argc, char * argv[]){
    // ── Argument parsing ─────────────────────────────────────────────────────
    if (parse_argument(argc, argv) != 0)
        return -1;
    
    cout << "Function: " << print_table(truth_table) << "\n";

    vector<minbool::MinTerm<NUM_BITS>> resp;
    string internal_symb, external_symb;
    if (isDNF) {
        resp = compute_minimal_DNF<NUM_BITS>(truth_table);
        internal_symb = "+";
        external_symb = "^";
        cout << "DNF: ";
    }
    else {
        resp = compute_minimal_CNF<NUM_BITS>(truth_table);
        internal_symb = "^";
        external_symb = "+";
        cout << "CNF: ";
    }
    for (minbool::MinTerm<NUM_BITS>& v : resp) {
        for (int i = 0; i < NUM_BITS; i++) {
            if (v[i] == minbool::MinTerm<NUM_BITS>::Dash)
                continue;
            else if (v[i] == minbool::MinTerm<NUM_BITS>::Zero)
                cout << "¬";
            cout << "x" << i+1 << " " << internal_symb << " ";
        }
        cout << "\n" << external_symb << "\n";
    }

    return 0;
}

int parse_argument(int argc, char * argv[]) {
    if (argc < 5) {
        print_usage(argv[0]);
        return -1;
    }
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-i" || arg == "--int") && i + 1 < argc) {
            truth_table = to_truthtable(stoull(argv[++i]), 32);
        } else if ((arg == "-s" || arg == "--string") && i + 1 < argc) {
            truth_table = to_truthtable(argv[++i], 32);
        } else if ((arg == "-f" || arg == "--form") && i + 1 < argc) {
            isDNF = (strcmp(argv[++i], "dnf") == 0);
        } else {
            cerr << "Unknown argument: " << arg << "\n";
            return -1;
        }
    }
    return 0;
}

void print_usage(const char* prog) {
    cerr << "Usage: " << prog << " -f [FORM] [OPTION] VALUE\n\n"
         << "Forms: "
         << "  - dnf"
         << "  - cnf"
         << "Options:\n"
         << "  -i, --int     Truth table as a 32-bit unsigned integer  (e.g. 363959363)\n"
         << "  -s, --str     Truth table as a binary string            (e.g. 00010110...)\n\n"
         << "Example:\n"
         << "  " << prog << " --int 363959363\n"
         << "  " << prog << " --string 00010110100110101110010001000011\n";
}