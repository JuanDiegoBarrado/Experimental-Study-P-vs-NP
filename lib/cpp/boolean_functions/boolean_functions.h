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

template <size_t Nbits>
vector<minbool::MinTerm<Nbits>> minimize(const truthtable& table) {
    using IntTypeN = typename minbool::MinTerm<Nbits>::IntTypeN;
    vector<IntTypeN> on_values, dc_values;

    for (uint32_t i = 0; i < table.size(); ++i)
        if (table[i])
            on_values.push_back((IntTypeN)i);  // cast aquí

    vector<minbool::MinTerm<Nbits>> solution = minbool::minimize_boolean<Nbits>(on_values, dc_values);
    return solution;
}

struct FNM {
    int var;
    bool sign;
    std::unique_ptr<FNM> left;
    std::unique_ptr<FNM> right;

    FNM(int var, bool sign, std::unique_ptr<FNM> left = nullptr, std::unique_ptr<FNM> right = nullptr)
        : var(var), sign(sign), left(std::move(left)), right(std::move(right))
    {}

    template <size_t Nbits>
    static std::unique_ptr<FNM> build(vector<minbool::MinTerm<Nbits>*>& minterms) {
        // Si no tiene mintérminos, la función es constante
        if (minterms.size() == 0)
            return nullptr;

        // Recorremos los mintérminos buscando la variable más significativa presente
        int max_var = -1, max_sign = -1;
        for (minbool::MinTerm<Nbits>* v : minterms) {
            for (int i = Nbits - 1; i >= 0; i--) {
                // Como vamos de detrás hacia alante, sabemos que el primero presente es la más significativa
                if ((*v)[i] != minbool::MinTerm<Nbits>::Dash) {
                    // Consideramos que es la más grande por el índice y no negada antes que negada
                    if (i > max_var || i == max_var && (*v)[i] > max_sign) {
                        max_var = i;
                        max_sign = (*v)[i];
                    }
                    break;
                }
            }
        }

        // Si todos los mintérminos estaban "apagados", la función es constante
        if (max_var == -1)
            return nullptr;
    
        // Particionamos los mintérminos en dos conjuntos: los que tienen la variable máxima y los que no
        vector<minbool::MinTerm<Nbits>*> presente, no_presente;
        for (minbool::MinTerm<Nbits>* v : minterms) {
            if ((*v)[max_var] == max_sign) {
                v->dash  |=  (1 << max_var);
                v->value &= ~(1 << max_var);
                presente.push_back(v);
            } else {
                no_presente.push_back(v);
            }
        }
    
        // construimos recursivamente ambas partes
        auto left  = build(presente);
        auto right = build(no_presente);
        return std::make_unique<FNM>(max_var + 1, (max_sign == 1), std::move(left), std::move(right));
    };

    std::string to_string() const {
        std::string sign_mark = this->sign ? "" : "¬"; 
        std::string left_str  = left  ? "^(" + left->to_string()  + ")" : "";
        std::string right_str = right ? " + " + right->to_string()      : "";
        return sign_mark + std::to_string(var) + left_str + right_str;
    }

    // std::string to_string() const {
    //     std::string sign_mark = this->sign ? "" : "¬"; 
    //     std::string left_str  = "^(" + (left ? left->to_string() : "&")  + ")";
    //     std::string right_str = " + " + (right ? right->to_string() : "!");
    //     return sign_mark + std::to_string(var) + left_str + right_str;
    // }

    std::string to_tree_string(const std::string& prefix = "", bool is_left = false) const {
        std::string result = prefix;
        result += (is_left ? "├(^)─" : "└(+)─");
        result += (this->sign ? "" : "¬");
        result += std::to_string(var) + "\n";

        std::string child_prefix = prefix + (is_left ? "│    " : "     "); // 4 espacios en vez de 3

        if (left)  result += left->to_tree_string(child_prefix, true);
        if (right) result += right->to_tree_string(child_prefix, false);

        return result;
    }

    ui64 uniqueness(unordered_map<string, ui64>& m) {
        if (left)
            left->uniqueness(m);
        if (right)
            right->uniqueness(m);

        string key = this->to_string();
        m[key]++;
        return m.size();
    }
};

template <size_t Nbits>
std::unique_ptr<FNM> fnm(const truthtable& sequence) {
    vector<minbool::MinTerm<Nbits>> minterms = minimize<Nbits>(sequence);
    vector<minbool::MinTerm<Nbits>*> minterms_ptrs;

    for (minbool::MinTerm<Nbits>& m : minterms)
        minterms_ptrs.push_back(&m);

    return FNM::build(minterms_ptrs);
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