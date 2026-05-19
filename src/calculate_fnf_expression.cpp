#include <vector>
#include <cstdint>
#include <cstring>

#include "boolean_functions/boolean_functions.h"
#include "boolean_functions/fourier_normal_form.h"
#include "metrics/metrics.h"

using ull = uint64_t;
enum Form {
    FNFD,
    FNFC,
    FNFG,
    INVALID
};

const int NUM_BITS = 5;
truthtable truth_table;
Form form = INVALID;


int parse_argument(int argc, char * argv[]);
void print_usage(const char* prog);

int main(int argc, char * argv[]){
    // ── Argument parsing ─────────────────────────────────────────────────────
    if (parse_argument(argc, argv) != 0)
        return -1;
    
    cout << "Function: " << print_table(truth_table) << "\n";

    string line, tree, form_type;
    ull score;
    std::unique_ptr<NodeFNF> exp;
    std::unordered_map<ui32, ui64> m;

    if (form == FNFD) {
        exp = compute_fnfd<NUM_BITS>(truth_table);
        score = distinct_fnfds<NUM_BITS>(truth_table);
        compute_fnfd<NUM_BITS>(truth_table)->compute_patterns(m);
        form_type = "FNFD";
    } else if (form == FNFC) {
        exp = compute_fnfc<NUM_BITS>(truth_table);
        score = distinct_fnfcs<NUM_BITS>(truth_table);
        form_type = "FNFC";
    } else {
        exp = compute_fnfg<NUM_BITS>(truth_table);
        score = distinct_fnfgs<NUM_BITS>(truth_table);
        form_type = "FNFG";
    }
    line = exp->to_string();
    tree = exp->to_tree_string();

    cout << form_type << ": " << line << "\n";
    cout << "Score: " << score << "\n";
    cout << "FNF tree:\n" << tree << "\n";
    cout << "FNFs:\n";
    for (pair<ui32, ui64> p : m) {
        truthtable t = to_truthtable(p.first);
        cout << "\t" << print_table(t) << "\n";
    }

    return 0;
}

int parse_argument(int argc, char * argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-i" || arg == "--int") && i + 1 < argc) {
            truth_table = to_truthtable(stoull(argv[++i]), 32);
        } else if ((arg == "-s" || arg == "--str") && i + 1 < argc) {
            truth_table = to_truthtable(argv[++i], 32);
        } else if ((arg == "-f" || arg == "--form") && i + 1 < argc) {
            i++;
            form = (strcmp(argv[i], "fnfd") == 0) ? FNFD : form;
            form = (strcmp(argv[i], "fnfc") == 0) ? FNFC : form;
            form = (strcmp(argv[i], "fnfg") == 0) ? FNFG : form;
        } else {
            print_usage(argv[0]);
            return -1;
        }
    }

    if (form == INVALID) {
        cerr << "Invalid form selected" << endl;
        print_usage(argv[0]);
        return -1;
    }
    return 0;
}

void print_usage(const char* prog) {
    cerr << "Usage: " << prog << " -f [FORM] [OPTION] VALUE\n\n"
         << "Forms: " << endl
         << "  - fnfd" << endl
         << "  - fnfc" << endl
         << "  - fnfg" << endl
         << "Options:\n"
         << "  -i, --int     Truth table as a 32-bit unsigned integer  (e.g. 363959363)\n"
         << "  -s, --str     Truth table as a binary string            (e.g. 00010110...)\n\n"
         << "Example:\n"
         << "  " << prog << " --int 363959363\n"
         << "  " << prog << " --string 00010110100110101110010001000011\n";
}