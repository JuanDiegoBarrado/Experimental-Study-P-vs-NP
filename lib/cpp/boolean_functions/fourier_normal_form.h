#pragma once

#include <vector>
#include <iostream>
#include <cstdint>
#include <bitset>
#include <cassert>
#include <memory>

#include "minbool/minbool.h"
#include "boolean_functions/boolean_functions.h"

const string AND_SYMB = "^";
const string OR_SYMB = "+";
const string NEG_SYMB = "¬";

const ui32 patterns[5] = {
    0xAAAAAAAA,  // x_1
    0xCCCCCCCC,  // x_2
    0xF0F0F0F0,  // x_3
    0xFF00FF00,  // x_4
    0xFFFF0000   // x_5
};

struct NodeFNF {
    int var;
    bool sign;
    string internal_symb;
    string external_symb;
    std::unique_ptr<NodeFNF> left;
    std::unique_ptr<NodeFNF> right;

    // Parámetros sin default PRIMERO, con default AL FINAL
    NodeFNF(int var, bool sign, 
            string internal_symb, 
            string external_symb,
            std::unique_ptr<NodeFNF> left = nullptr, 
            std::unique_ptr<NodeFNF> right = nullptr)
        : var(var), sign(sign), 
          internal_symb(internal_symb), 
          external_symb(external_symb), 
          left(std::move(left)), 
          right(std::move(right))
    {}

    virtual ~NodeFNF() = default;
    virtual std::string to_string() const = 0;

    std::string to_tree_string(const std::string& prefix = "", bool is_left = false) const {
        std::string result = prefix;
        result += (is_left ? ("├(" + this->internal_symb + ")─") : ("└(" + this->external_symb + ")─"));
        result += (this->sign ? "" : NEG_SYMB);
        result += std::to_string(var) + "\n";

        std::string child_prefix = prefix + (is_left ? "│    " : "     ");

        if (left)  result += left->to_tree_string(child_prefix, true);
        if (right) result += right->to_tree_string(child_prefix, false);

        return result;
    }

    virtual ui64 compute_patterns(unordered_map<ui32, ui64>& m) const = 0;
};

struct NodeFNFD : NodeFNF {
    NodeFNFD(int var, bool sign, 
             std::unique_ptr<NodeFNF> left = nullptr, 
             std::unique_ptr<NodeFNF> right = nullptr) 
        : NodeFNF(var, sign, AND_SYMB, OR_SYMB, std::move(left), std::move(right))
    {
        // Cuerpo vacío - todo se hace en la lista de inicialización
    }
    
    std::string to_string() const override {
        std::string sign_mark = this->sign ? "" : NEG_SYMB; 
        std::string left_str  = left  ? (this->internal_symb + "(") + left->to_string()  + ")" : "";
        std::string right_str = right ? (" " + this->external_symb + " ") + right->to_string()      : "";
        return sign_mark + std::to_string(var) + left_str + right_str;
    }

    ui64 compute_patterns(unordered_map<ui32, ui64>& m) const override {
        ui32 pattern = patterns[this->var - 1];
        ui32 pattern_l, pattern_d;
        if (!this->sign)
            pattern = ~pattern;
        if (left) {
            pattern_l = left->compute_patterns(m);
            pattern = pattern & pattern_l;
        }
        if (right) {
            pattern_d = right->compute_patterns(m);
            pattern = pattern | pattern_d;
        }

        m[pattern]++;
        return pattern;
    };
};

struct NodeFNFC : NodeFNF {
    NodeFNFC(int var, bool sign, 
             std::unique_ptr<NodeFNF> left = nullptr, 
             std::unique_ptr<NodeFNF> right = nullptr) 
        : NodeFNF(var, sign, OR_SYMB, AND_SYMB, std::move(left), std::move(right))
    {}
    
    std::string to_string() const override {
        std::string sign_mark = this->sign ? "" : NEG_SYMB; 
        std::string left_str  = left  ? (this->internal_symb + "(") + left->to_string()  + ")" : "";
        std::string right_str = right ? (" " + this->external_symb + " ") + right->to_string()      : "";
        return "(" + sign_mark + std::to_string(var) + left_str + ")" + right_str;
    }

    ui64 compute_patterns(unordered_map<ui32, ui64>& m) const override {
        ui32 pattern = patterns[this->var - 1];
        ui32 pattern_l, pattern_d;
        if (!this->sign)
            pattern = ~pattern;
        if (left) {
            pattern_l = left->compute_patterns(m);
            pattern = pattern | pattern_l;
        }
        if (right) {
            pattern_d = right->compute_patterns(m);
            pattern = pattern & pattern_d;
        }

        m[pattern]++;
        return pattern;
    }
};

template <size_t Nbits>
truthtable minterms_to_truthtable(vector<minbool::MinTerm<Nbits>*>& terms, bool isMinTerms) {
    static_assert(Nbits <= MAX_INPUT_BITS, "Nbits exceeds truthtable supported size");
    static_assert(Nbits < sizeof(size_t) * 8, "Nbits too large for size_t shift");

    const size_t table_size = size_t(1) << Nbits;
    truthtable table(table_size, !isMinTerms);

    for (minbool::MinTerm<Nbits>* term_ptr : terms) {
        if (term_ptr == nullptr)
            continue;

        term_ptr->foreach_value([&table, isMinTerms](typename minbool::MinTerm<Nbits>::IntTypeN value) {
            table[static_cast<size_t>(value)] = isMinTerms;
        });
    }

    return table;
}

template <typename NodeType, size_t Nbits>
static std::unique_ptr<NodeType> build_homogeneous(vector<minbool::MinTerm<Nbits>*>& terms) {
    // Si no tiene términos, la función es constante
    if (terms.size() == 0)
        return nullptr;

    // Recorremos los términos buscando la variable más significativa presente
    int max_var = -1, max_sign = -1;
    for (minbool::MinTerm<Nbits>* v : terms) {
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

    // Si todos los términos estaban "apagados", la función es constante
    if (max_var == -1)
        return nullptr;

    // Particionamos los términos en dos conjuntos: los que tienen la variable máxima y los que no
    vector<minbool::MinTerm<Nbits>*> presente, no_presente;
    for (minbool::MinTerm<Nbits>* v : terms) {
        if ((*v)[max_var] == max_sign) {
            v->dash  |=  (1 << max_var); // dont care a 1
            v->value &= ~(1 << max_var); // bit de signo a 0 (por convencion)
            presente.push_back(v);
        } else {
            no_presente.push_back(v);
        }
    }

    // construimos recursivamente ambas partes
    auto left  = build_homogeneous<NodeType, Nbits>(presente);
    auto right = build_homogeneous<NodeType, Nbits>(no_presente);
    return std::make_unique<NodeType>(max_var + 1, (max_sign == 1), std::move(left), std::move(right));
};

template <size_t Nbits>
static std::unique_ptr<NodeFNF> build_heterogeneous(const truthtable& truth_table) {
    vector<minbool::MinTerm<Nbits>> minTerms = compute_minimal_DNF<Nbits>(truth_table);
    vector<minbool::MinTerm<Nbits>> maxTerms = compute_minimal_CNF<Nbits>(truth_table);
    int max_var = -1, max_sign = -1;
    vector<minbool::MinTerm<Nbits>*> presente, no_presente;
    truthtable t_presente, t_no_presente;

    // Calculamos las subfunciones de separar en DNF
    // Si no tiene mintérminos, la función es constante
    if (minTerms.size() == 0)
        return nullptr;

    // Recorremos los mintérminos buscando la variable más significativa presente
    max_var = -1;
    max_sign = -1;
    for (minbool::MinTerm<Nbits>& v : minTerms) {
        for (int i = Nbits - 1; i >= 0; i--) {
            // Como vamos de detrás hacia alante, sabemos que el primero presente es la más significativa
            if (v[i] != minbool::MinTerm<Nbits>::Dash) {
                // Consideramos que es la más grande por el índice y no negada antes que negada
                if (i > max_var || i == max_var && v[i] > max_sign) {
                    max_var = i;
                    max_sign = v[i];
                }
                break;
            }
        }
    }

    // Si todos los mintérminos estaban "apagados", la función es constante
    if (max_var == -1)
        return nullptr;

    // Particionamos los mintérminos en dos conjuntos: los que tienen la variable máxima y los que no
    presente.clear();
    no_presente.clear();
    for (minbool::MinTerm<Nbits>& v : minTerms) {
        if (v[max_var] == max_sign) {
            v.dash  |=  (1 << max_var); // dont care a 1
            v.value &= ~(1 << max_var); // bit de signo a 0 (por convencion)
            presente.push_back(&v);
        } else {
            no_presente.push_back(&v);
        }
    }    
    // consideramos las partes que quedarían si dividimos en DNF -> x_i^(f_1) + (f_2)
    // f_1 hay que calcular de nuevo su tabla de verdad, f_2 es duplicar donde x_i sea 0.
    t_presente = minterms_to_truthtable<Nbits>(presente, true);
    t_no_presente = minterms_to_truthtable<Nbits>(no_presente, true);
    auto dnf_left  = build_heterogeneous<Nbits>(t_presente);
    auto dnf_right = build_heterogeneous<Nbits>(t_no_presente);

    // Calculamos las subfunciones de separar en CNF
        // Si no tiene maxtérminos, la función es constante
    if (maxTerms.size() == 0)
        return nullptr;

    // Recorremos los maxtérminos buscando la variable más significativa presente
    max_var = -1;
    max_sign = -1;
    for (minbool::MinTerm<Nbits>& v : maxTerms) {
        for (int i = Nbits - 1; i >= 0; i--) {
            // Como vamos de detrás hacia alante, sabemos que el primero presente es la más significativa
            if (v[i] != minbool::MinTerm<Nbits>::Dash) {
                // Consideramos que es la más grande por el índice y no negada antes que negada
                if (i > max_var || i == max_var && v[i] > max_sign) {
                    max_var = i;
                    max_sign = v[i];
                }
                break;
            }
        }
    }

    // Si todos los maxtérminos estaban "apagados", la función es constante
    if (max_var == -1)
        return nullptr;

    // Particionamos los maxtérminos en dos conjuntos: los que tienen la variable máxima y los que no
    presente.clear();
    no_presente.clear();
    for (minbool::MinTerm<Nbits>& v : maxTerms) {
        if (v[max_var] == max_sign) {
            v.dash  |=  (1 << max_var); // dont care a 1
            v.value &= ~(1 << max_var); // bit de signo a 0 (por convencion)
            presente.push_back(&v);
        } else {
            no_presente.push_back(&v);
        }
    }    
    // consideramos las partes que quedarían si dividimos en DNF
    t_presente = minterms_to_truthtable<Nbits>(presente, false);
    t_no_presente = minterms_to_truthtable<Nbits>(no_presente, false);
    auto cnf_left  = build_heterogeneous<Nbits>(t_presente);
    auto cnf_right = build_heterogeneous<Nbits>(t_no_presente);

    unordered_map<ui32, ui64> dnf_m, cnf_m;
    if (dnf_left != nullptr)
        dnf_left->compute_patterns(dnf_m);
    if (dnf_right != nullptr)
        dnf_right->compute_patterns(dnf_m);
    if (cnf_left != nullptr)
        cnf_left->compute_patterns(cnf_m);
    if (cnf_right != nullptr)
        cnf_right->compute_patterns(cnf_m);

    if (dnf_m.size() < cnf_m.size())
        return std::make_unique<NodeFNFC>(max_var + 1, (max_sign == 1), std::move(cnf_left), std::move(cnf_right));
    else
        return std::make_unique<NodeFNFD>(max_var + 1, (max_sign == 1), std::move(dnf_left), std::move(dnf_right));
};

template <size_t Nbits>
std::unique_ptr<NodeFNFC> compute_fnfc(const truthtable& sequence) {
    vector<minbool::MinTerm<Nbits>> maxterms = compute_minimal_CNF<Nbits>(sequence);
    vector<minbool::MinTerm<Nbits>*> maxterms_ptrs;

    for (minbool::MinTerm<Nbits>& m : maxterms)
        maxterms_ptrs.push_back(&m);

    return build_homogeneous<NodeFNFC, Nbits>(maxterms_ptrs);
}

template <size_t Nbits>
std::unique_ptr<NodeFNFD> compute_fnfd(const truthtable& sequence) {
    vector<minbool::MinTerm<Nbits>> minterms = compute_minimal_DNF<Nbits>(sequence);
    vector<minbool::MinTerm<Nbits>*> minterms_ptrs;

    for (minbool::MinTerm<Nbits>& m : minterms)
        minterms_ptrs.push_back(&m);

    return build_homogeneous<NodeFNFD, Nbits>(minterms_ptrs);
}

template <size_t Nbits>
std::unique_ptr<NodeFNF> compute_fnfg(const truthtable& sequence) {
    return build_heterogeneous<Nbits>(sequence);
}
