#include "optimal5.h"
#include <unordered_map>

typedef std::pair<uint64_t, std::string> lengthpath;

lengthpath held_karp(std::vector<std::vector<uint64_t> > dmatrix) {
    /*
    * Find the minimum-distance Hamiltonian path starting at vertex
    * A and visiting (in some order) vertices {B, C, D, ...}. Only
    * recommended for paths of 24 or fewer vertices.
    *
    * The result is an ordered pair of the length and an uppercase
    * alphabetic string giving the sequence of vertices to visit in
    * order.
    */

    uint8_t n = dmatrix.size();
    uint64_t maxint = 1ull << n;

    std::unordered_map<uint64_t, lengthpath> karps;

    for (uint64_t vm = 2; vm < maxint; vm += 2) {

        if ((vm & (vm - 1)) == 0) {
            // Base case: one city

            uint64_t dst = __builtin_ctzll(vm);
            uint64_t l = dmatrix[0][dst];

            uint64_t idx = (vm << 6) + dst;

            karps.emplace(idx, lengthpath(l, std::string(1, 'A') + std::string(1, 'A' + dst)));

        } else {
            // Inductive step: multiple cities

            uint64_t um = vm;
            while (um != 0) {
                uint64_t powof2 = um & (-um);
                um ^= powof2;

                std::string best_path;
                uint64_t dst = __builtin_ctzll(powof2);
                uint64_t idx = (vm << 6) + dst;

                uint64_t wm = vm ^ powof2;

                uint64_t l = -1; // will wrap around to maximum uint64

                for (uint64_t src = 0; src < n; src++) {
                    if (((wm >> src) & 1) == 0) { continue; }
                    uint64_t parent_idx = (wm << 6) + src;
                    uint64_t k = dmatrix[src][dst] + karps[parent_idx].first;
                    if (k < l) {
                        best_path = karps[parent_idx].second + std::string(1, 'A' + dst);
                        l = k;
                    }
                }

                karps.emplace(idx, lengthpath(l, best_path));
            }
        }
    }

    // We should now have the shortest path.

    lengthpath optimal(-1, "NONSENSE");

    for (uint64_t i = 1; i < n; i++) {
        auto contender = karps[((maxint - 2) << 6) + i];
        if (contender.first < optimal.first) {
            optimal = contender;
        }
    }

    return optimal;

}


int main(int argc, char* argv[]) {

    uint16_t min4[51200];
    opt5::prepare_table(min4);

    auto cayley = opt5::get_cayley();
    auto reps   = cayley[0];
    std::vector<uint16_t> invreps(32768);
    std::vector<std::vector<uint8_t> > min_paths(384);
    for (int z = 0; z < 384; z++) {
        uint32_t w = reps[z];
        invreps[w] = z;
        int m = 0;
        do {
            m = ((uint8_t*) (min4 + 32768))[w];
            w = opt5::apply_move(w << 16, m) >> 16;
            if (m != 0) { min_paths[z].push_back(m); }
        } while (m != 0);
    }

    std::vector<uint16_t> inverses;
    for (int i = 0; i < 384; i++) {
        for (int j = 0; j < 384; j++) {
            cayley[i][j] = invreps[cayley[i][j]];
            if (cayley[i][j] == 0) { inverses.push_back(j); }
        }
    }

    std::map<std::vector<int>, int> sols;

    std::vector<int> things(1984);

    int next_thing = 2;

    for (uint64_t i = 0; i < 32768; i++) {
        if (min4[i] == i) {
            auto x = opt5::stabilise_circuit_4(i);

            if (x.size() == 1) {
                things[i % 1984] = 1;
            } else if (x.size() > 24) {
                things[i % 1984] = 0;
            } else {

                if (sols.count(x) == 0) {
                    sols[x] = next_thing;
                    std::vector<std::vector<uint64_t> > dmatrix(x.size());
                    for (uint64_t j = 0; j < x.size(); j++) {
                        for (uint64_t k = 0; k < x.size(); k++) {
                            uint64_t dist = min_paths[cayley[x[j]][inverses[x[k]]]].size();
                            // printf("%d ", dist);
                            dmatrix[j].push_back(dist);
                        }
                        // printf("\n");
                    }
                    auto hk = held_karp(dmatrix);

                    std::cout << "void hp_path" << next_thing;
                    std::cout << "(uint32_t &t, uint32_t &tt, int16_t *perm, int16_t *element) {" << std::endl;
                    std::cout << "    uint32_t d = 0;" << std::endl;

                    for (uint64_t j = 1; j < hk.second.size(); j++) {
                        auto vec = min_paths[cayley[x[hk.second[j-1]-'A']][inverses[x[hk.second[j]-'A']]]];
                        for (auto it = vec.begin(); it != vec.end(); ++it) {
                            uint8_t move = *it;
                            switch (move) {
                                case 0:
                                break;
                                case 1:  std::cout << "    d = (t ^ (t >> 4)) & 0x00f000f0; t ^= d + (d << 4); PERM_SWAP(2, 3);\n";
                                break;
                                case 2:  std::cout << "    d = (t ^ (t >> 2)) & 0x0c0c0c0c; t ^= d + (d << 2); PERM_SWAP(3, 4);\n";
                                break;
                                case 3:  std::cout << "    d = (t ^ (t >> 1)) & 0x22222222; t ^= d + (d << 1); PERM_SWAP(4, 5);\n";
                                break;
                                case 4:  std::cout << "    d = (t ^ (t >> 6)) & 0x00cc00cc; t ^= d + (d << 6); PERM_SWAP(2, 4);\n";
                                break;
                                case 5:  std::cout << "    d = (t ^ (t >> 3)) & 0x0a0a0a0a; t ^= d + (d << 3); PERM_SWAP(3, 5);\n";
                                break;
                                case 6:  std::cout << "    d = (t ^ (t >> 7)) & 0x00aa00aa; t ^= d + (d << 7); PERM_SWAP(2, 5);\n";
                                break;
                                case 7:
                                case 8:  std::cout << "    t = ((t & 0x00ff00ffu) << 8) + ((t & 0xff00ff00u) >> 8); perm[2] = -perm[2];\n";
                                break;
                                case 9:
                                case 10: std::cout << "    t = ((t & 0x0f0f0f0fu) << 4) + ((t & 0xf0f0f0f0u) >> 4); perm[3] = -perm[3];\n";
                                break;
                                case 11:
                                case 12: std::cout << "    t = ((t & 0x33333333u) << 2) + ((t & 0xccccccccu) >> 2); perm[4] = -perm[4];\n";
                                break;
                                case 13:
                                case 14: std::cout << "    t = ((t & 0x55555555u) << 1) + ((t & 0xaaaaaaaau) >> 1); perm[5] = -perm[5];\n";
                                break;
                            }
                        }
                        std::cout << "    if (t & 0x80000000u) { t = ~t; perm[0] ^= 1; }" << std::endl;
                        std::cout << "    if (t < tt) { memcpy(element, perm, 6 * sizeof(uint16_t)); tt = t; }" << std::endl;
                    }
                    std::cout << "}\n" << std::endl;

                    next_thing += 1;
                }
                things[i % 1984] = sols[x];
            }
        }
    }

    std::cout << "const static uint8_t hpaths[1984] = {\n";
    for (int i = 0; i < 1984; i++) {
        if ((i % 31) == 0) { std::cout << "    "; }
        std::cout << things[i];
        std::cout << ((i == 1983) ? "};" : ",");
        std::cout << (((i % 31) == 30) ? "\n" : " ");
    }
    std::cout << std::endl;

    std::cout << "const static hpath fpointers[] = {hp_exhaust, hp_null";
    for (int i = 2; i < next_thing; i++) {
        std::cout << ", hp_path" << i;
    }
    std::cout << "};\n" << std::endl;

    return 0;

}
