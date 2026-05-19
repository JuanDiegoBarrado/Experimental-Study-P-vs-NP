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
ull num_classifications(const truthtable& sequence);

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

const std::unordered_map<std::string, complexity_metric> METRICS = {
    { "num_ones",  num_ones  },
    { "num_classifications", num_classifications },
    { "distinct_fnfds", distinct_fnfds<5> },
    { "distinct_fnfcs", distinct_fnfcs<5> },
    { "distinct_fnfgs", distinct_fnfgs<5> }
};