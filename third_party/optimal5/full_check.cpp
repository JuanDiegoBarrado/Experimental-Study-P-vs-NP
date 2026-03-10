#include "optimal5.h"

int main(int argc, char* argv[]) {

    (void) argc;
    (void) argv;

    opt5::Optimiser opt("knuthies.dat");

    uint32_t functions[16] = {0};
    uint32_t classes[16] = {0};

    for (auto it = opt.chains.begin(); it != opt.chains.end(); ++it) {
        classes[it->second.n_gates] += 1;
    }

    for (uint64_t v = 0; v < 0x100000000ull; v++) {

        auto gv = opt.lookup(v);
        functions[gv.n_gates] += 1;

        if ((v & 0xffffff) == 0xffffff) {
            std::cerr << (v + 1) << " chains completed." << std::endl;
        }

    }

    for (int i = 0; i < 16; i++) {
        printf("%2d: %10d %10d\n", i, classes[i], functions[i]);
    }

    return 0;

}
