#include "optimal5/optimal5.h"

const uint64_t LAST_FUNC = 0x100000000ull;

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    opt5::Optimiser opt("third_party/optimal5/knuthies.dat");

    std::ofstream file("data/data.bin", std::ios::out | std::ios::binary);
    std::vector<char> buffer(1024 * 1024);
    file.rdbuf()->pubsetbuf(buffer.data(), buffer.size());

    for(uint64_t i = 0; i < LAST_FUNC; ++i) {
        auto gv = opt.lookup(i);
        uint8_t value = gv.n_gates;
        file.put(value);

        if ((i & 0xffffff) == 0xffffff)
            std::cerr << (i + 1) << " chains completed." << std::endl;
    }
    file.close();

    return 0;
}
