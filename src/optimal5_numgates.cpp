#include "optimal5/optimal5.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: ./" << argv[0] << " 0x169ae443" << std::endl;
        return 1;
    }

    std::string filename("third_party/optimal5/knuthies.dat");
    std::string line = argv[1];

    opt5::Optimiser opt(filename);
    uint32_t fn = std::stoll(line, 0, 0);
    auto gv = opt.lookup(fn);
    int num_gates = gv.n_gates;
    std::cout << "Num gates: " << num_gates << "\n";

    return 0;

}
