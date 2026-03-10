#include "optimal5.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: ./human_readable knuthies.dat" << std::endl;
        return 1;
    }

    std::string filename(argv[1]);
    opt5::Optimiser opt(filename);

    std::cerr << "Enter truth tables, one on each line, e.g. 0x169ae443" << std::endl;

    std::string line;

    while (std::getline(std::cin, line)) {
        uint32_t fn = std::stoll(line, 0, 0);
        auto gv = opt.lookup(fn);
        std::string s = opt5::print_gatevec(gv);
        std::cout << s << std::endl;
    }

    return 0;

}
