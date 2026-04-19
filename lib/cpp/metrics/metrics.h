#include "boolean_functions/boolean_functions.h"
#include <functional>
#include <memory>
#include <string>
#include <cstdint>

using namespace std;
using ull = uint64_t;
using complexity_metric = std::function<ull(const truthtable&)>;

ull num_ones(const truthtable& sequence);
ull vc_values(const truthtable& sequence);

template <size_t Nbits>
ull distinct_fnms(const truthtable& sequence) {
    std::unique_ptr<FNM> f = fnm<Nbits>(sequence);
    ull score = -1;
    if (f) {
        std::unique_ptr<unordered_map<string, ull>> m = make_unique<unordered_map<string, ull>>();
        score = f->uniqueness(*m);
    }
    else
        score = 0;
    return score;
}

template <size_t Nbits>
ull neg_impact_on_fnms(const truthtable& sequence) {
    truthtable neg_sequence = sequence;
    for (ull i = 0; i < neg_sequence.size(); i++)
        neg_sequence[i] = !neg_sequence[i];
    
    std::unique_ptr<FNM> f = fnm<Nbits>(sequence);
    std::unique_ptr<FNM> neg_f = fnm<Nbits>(neg_sequence);
    ull fscore = -1, neg_fscore = -1;
    if (f) {
        std::unique_ptr<unordered_map<string, ull>> m = make_unique<unordered_map<string, ull>>();
        fscore = f->uniqueness(*m);
    }
    else
        fscore = 0;
    if (neg_f) {
        std::unique_ptr<unordered_map<string, ull>> m = make_unique<unordered_map<string, ull>>();
        neg_fscore = neg_f->uniqueness(*m);
    }
    else
        neg_fscore = 0;
    
    return (neg_fscore > fscore) ? (neg_fscore - fscore) : (fscore - neg_fscore);
}

const std::unordered_map<std::string, complexity_metric> METRICS = {
    { "num_ones",  num_ones  },
    { "vc_values", vc_values },
    { "distinct_fnms", distinct_fnms<5> },
    { "neg_impact_on_fnms", neg_impact_on_fnms<5> }
};