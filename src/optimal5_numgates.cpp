#include "optimal5/optimal5.h"

uint32_t fn = 0;

void print_usage(const char* prog);
int parse_arguments(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    if (parse_arguments(argc, argv) != 0)
        return -1;

    std::string filename("third_party/optimal5/knuthies.dat");

    opt5::Optimiser opt(filename);
    auto gv = opt.lookup(fn);
    int num_gates = gv.n_gates;
    std::cout << "Num gates: " << num_gates << "\n";

    return 0;

}

void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [-i|--int VALUE] [-h|--hex VALUE] [-b|--bin VALUE]\n"
              << "  -i, --int   Valor en decimal (ej: 363959363)\n"
              << "  -h, --hex   Valor en hexadecimal (ej: 0x169ae443)\n"
              << "  -b, --bin   Valor en binario (ej: 10101100...)\n";
}

int parse_arguments(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-i" || arg == "--int") && i + 1 < argc) {
            fn = static_cast<uint32_t>(std::stoll(argv[++i], nullptr, 10));
        } else if ((arg == "-h" || arg == "--hex") && i + 1 < argc) {
            fn = static_cast<uint32_t>(std::stoll(argv[++i], nullptr, 0));
        } else if ((arg == "-b" || arg == "--bin") && i + 1 < argc) {
            fn = static_cast<uint32_t>(std::stoll(argv[++i], nullptr, 2));
        } else {
            print_usage(argv[0]);
            return -1;
        }
    }
    return 0;
}