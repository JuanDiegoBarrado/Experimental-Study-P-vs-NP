#include "boolean_functions/boolean_functions.h"
#include <functional>
#include <memory>
#include <string>

using namespace std;
using ull = unsigned long long int;
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

const std::unordered_map<std::string, complexity_metric> METRICS = {
    { "num_ones",  num_ones  },
    { "vc_values", vc_values },
    { "distinct_fnms", distinct_fnms<5> }
};