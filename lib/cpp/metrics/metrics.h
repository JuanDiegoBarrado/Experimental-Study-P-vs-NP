#include <functional>
#include <memory>
#include <string>
#include <cstdint>

#include "boolean_functions/boolean_functions.h"
#include "boolean_functions/fourier_normal_form.h"

using namespace std;
using ull = uint64_t;
using complexity_metric = std::function<ull(const truthtable&)>;

ull num_ones(const truthtable& sequence);
ull vc_values(const truthtable& sequence);

template <size_t Nbits>
ull distinct_fnfds(const truthtable& sequence) {
    std::unique_ptr<NodeFNFD> f = compute_fnfd<Nbits>(sequence);
    ull score = -1;
    if (f) {
        std::unordered_map<ui32, ui64> m;
        f->compute_patterns(m);
        score = m.size();
    }
    else
        score = 0;
    return score;
}

template <size_t Nbits>
ull distinct_fnfcs(const truthtable& sequence) {
    std::unique_ptr<NodeFNFC> f = compute_fnfc<Nbits>(sequence);
    ull score = -1;
    if (f) {
        std::unordered_map<ui32, ui64> m;
        f->compute_patterns(m);
        score = m.size();
    }
    else
        score = 0;
    return score;
}

template <size_t Nbits>
ull distinct_fnfgs(const truthtable& sequence) {
    std::unique_ptr<NodeFNF> f = compute_fnfg<Nbits>(sequence);
    ull score = -1;
    if (f) {
        std::unordered_map<ui32, ui64> m;
        f->compute_patterns(m);
        score = m.size();
    }
    else
        score = 0;
    return score;
}

template <size_t Nbits>
ull neg_impact_on_fnfds(const truthtable& sequence) {
    truthtable neg_sequence = sequence;
    for (ull i = 0; i < neg_sequence.size(); i++)
        neg_sequence[i] = !neg_sequence[i];
    
    std::unique_ptr<NodeFNFD> f = compute_fnfd<Nbits>(sequence);
    std::unique_ptr<NodeFNFD> neg_f = compute_fnfd<Nbits>(neg_sequence);
    ull fscore = -1, neg_fscore = -1;
    if (f) {
        std::unordered_map<ui32, ui64> m;
        f->compute_patterns(m);
        fscore = m.size();
    }
    else
        fscore = 0;
    if (neg_f) {
        std::unordered_map<ui32, ui64> m;
        neg_f->compute_patterns(m);
        neg_fscore = m.size();
    }
    else
        neg_fscore = 0;
    
    return (neg_fscore > fscore) ? (neg_fscore - fscore) : (fscore - neg_fscore);
}

const std::unordered_map<std::string, complexity_metric> METRICS = {
    { "num_ones",  num_ones  },
    { "vc_values", vc_values },
    { "distinct_fnfds", distinct_fnfds<5> },
    { "distinct_fnfcs", distinct_fnfcs<5> },
    { "distinct_fnfgs", distinct_fnfgs<5> },
    { "neg_impact_on_fnfds", neg_impact_on_fnfds<5> }
};