#include <bits/stdc++.h>
#define MAX_INPUTS 12
#define MAX_LENGTH 2**12

using namespace std;
using vb = bitset<MAX_INPUTS>;

void calculate(void (*func)(vb setting), const vb& setting) {
    func(setting);
    return;
}
