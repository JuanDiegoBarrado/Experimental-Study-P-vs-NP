#include "visualization/terminal_formatter.h"

std::string format_time(double seconds) {
    int total = static_cast<int>(seconds);

    int hours = total / 3600;
    int minutes = (total % 3600) / 60;
    int secs = total % 60;

    std::ostringstream oss;

    if (hours > 0)
        oss << hours << "h ";

    if (minutes > 0 || hours > 0)
        oss << minutes << "m ";

    oss << secs << "s";

    return oss.str();
}