#include <string>
#include <sstream>
#include <iomanip>

#define CLEAR_LINE "\033[2K\r"
#define CLEAR_LINES(n) \
    for(int _i = 0; _i < (n)-1; ++_i) cout << "\033[A\033[2K"; \
    cout << "\033[2K\r";

std::string format_time(double seconds);