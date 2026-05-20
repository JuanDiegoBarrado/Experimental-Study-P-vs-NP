#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <cassert>
#include <unordered_map>

#include "boolean_functions/boolean_functions.h"
#include "boolean_functions/fourier_normal_form.h"
#include "minbool/minbool.h"

using namespace std;

// Helper to check if DNF/CNF is correct
template <size_t Nbits>
bool check_dnf(const truthtable& tt, const vector<minbool::MinTerm<Nbits>>& dnf) {
    vector<minbool::MinTerm<Nbits>*> ptrs;
    for (auto& m : const_cast<vector<minbool::MinTerm<Nbits>>&>(dnf)) {
        ptrs.push_back(&m);
    }
    truthtable eval_tt = minterms_to_truthtable<Nbits>(ptrs, true);
    for(size_t i=0; i<tt.size(); i++){
        if(tt[i] != eval_tt[i]) return false;
    }
    return true;
}

template <size_t Nbits>
bool check_cnf(const truthtable& tt, const vector<minbool::MinTerm<Nbits>>& cnf) {
    vector<minbool::MinTerm<Nbits>*> ptrs;
    for (auto& m : const_cast<vector<minbool::MinTerm<Nbits>>&>(cnf)) {
        ptrs.push_back(&m);
    }
    truthtable eval_tt = minterms_to_truthtable<Nbits>(ptrs, false);
    for(size_t i=0; i<tt.size(); i++){
        if(tt[i] != eval_tt[i]) return false;
    }
    return true;
}

void test_1_boolean_functions() {
    cout << "Running Test 1: boolean_functions basics..." << endl;
    
    // to_truthtable int
    truthtable tt1 = to_truthtable((uint32_t)6, 3);
    assert(tt1.size() == 3);
    assert(tt1[0] == 0);
    assert(tt1[1] == 1);
    assert(tt1[2] == 1);

    // to_truthtable string
    truthtable tt2 = to_truthtable("110", 3);
    assert(tt2.size() == 3);
    assert(tt2[0] == 0);
    assert(tt2[1] == 1);
    assert(tt2[2] == 1);

    // DNF and CNF checks
    // We will use 5 variables to be consistent with patterns
    for (uint32_t func = 0; func < 100; func++) {
        truthtable tt = to_truthtable(func * 12345, 32); // pseudo-random func
        auto dnf = compute_minimal_DNF<5>(tt);
        assert(check_dnf<5>(tt, dnf));
        
        auto cnf = compute_minimal_CNF<5>(tt);
        assert(check_cnf<5>(tt, cnf));
    }
    cout << "Test 1 passed!" << endl;
}

bool check_tree_vars(const NodeFNF* node, int parent_var) {
    if (node == nullptr) return true;
    if (node->var > parent_var) {
        cout << "FAILED: node->var (" << node->var << ") > parent_var (" << parent_var << ")" << endl;
        return false;
    }
    
    // The children can have max_var <= node->var, actually wait, shouldn't they have < parent_var?
    // Let's just check they don't have variables > node->var. The problem statement says:
    // "F_1 y F_2 no tienen las variables mayores que x_5"
    // So if root is x_5, children must have vars <= 5. Wait, if F_2 has x_5, then it's <= 5.
    // If we require node->var <= parent_var, it means no child can have a var greater than its parent.
    // So let's recursively pass node->var as the new parent_var.
    if (!check_tree_vars(node->left.get(), node->var)) return false;
    if (!check_tree_vars(node->right.get(), node->var)) return false;
    
    return true;
}

void test_fnfd() {
    cout << "Running Test: fourier_normal_form decomposition (FNFD)..." << endl;
    
    for (uint32_t func = 0; func < 1000; func++) {
        uint32_t func_int = (func * 13579) ^ (func << 16);
        truthtable tt = to_truthtable(func_int, 32);
        
        auto root = compute_fnfd<5>(tt);
        
        if (root) {
            if (!check_tree_vars(root.get(), 5)) {
                cout << "Failing tree (FNFD):\n" << root->to_tree_string() << endl;
                assert(false);
            }
            unordered_map<ui32, ui64> m;
            ui64 computed_pattern = root->compute_patterns(m);
            if ((computed_pattern & 0xFFFFFFFF) != func_int) {
                cout << "Failing func_int (FNFD): " << func_int << " computed: " << computed_pattern << endl;
                assert(false);
            }
        } else {
            assert(func_int == 0 || func_int == 0xFFFFFFFF);
        }
    }
    cout << "Test FNFD passed!" << endl;
}

void test_fnfc() {
    cout << "Running Test: fourier_normal_form decomposition (FNFC)..." << endl;
    
    for (uint32_t func = 0; func < 1000; func++) {
        uint32_t func_int = (func * 13579) ^ (func << 16);
        truthtable tt = to_truthtable(func_int, 32);
        
        auto root = compute_fnfc<5>(tt);
        
        if (root) {
            if (!check_tree_vars(root.get(), 5)) {
                cout << "Failing tree (FNFC):\n" << root->to_tree_string() << endl;
                assert(false);
            }
            unordered_map<ui32, ui64> m;
            ui64 computed_pattern = root->compute_patterns(m);
            if ((computed_pattern & 0xFFFFFFFF) != func_int) {
                cout << "Failing func_int (FNFC): " << func_int << " computed: " << computed_pattern << endl;
                assert(false);
            }
        } else {
            assert(func_int == 0 || func_int == 0xFFFFFFFF);
        }
    }
    cout << "Test FNFC passed!" << endl;
}

void test_fnfg() {
    cout << "Running Test: fourier_normal_form decomposition (FNFG)..." << endl;
    
    for (uint32_t func = 0; func < 1000; func++) {
        uint32_t func_int = (func * 13579) ^ (func << 16);
        truthtable tt = to_truthtable(func_int, 32);
        
        auto root = compute_fnfg<5>(tt);
        
        if (root) {
            if (!check_tree_vars(root.get(), 5)) {
                cout << "Failing tree (FNFG):\n" << root->to_tree_string() << endl;
                assert(false);
            }
            unordered_map<ui32, ui64> m;
            ui64 computed_pattern = root->compute_patterns(m);
            if ((computed_pattern & 0xFFFFFFFF) != func_int) {
                cout << "Failing func_int (FNFG): " << func_int << " computed: " << computed_pattern << endl;
                assert(false);
            }
        } else {
            assert(func_int == 0 || func_int == 0xFFFFFFFF);
        }
    }
    cout << "Test FNFG passed!" << endl;
}

int main() {
    test_1_boolean_functions();
    test_fnfd();
    test_fnfc();
    test_fnfg();
    cout << "ALL TESTS PASSED!" << endl;
    return 0;
}
