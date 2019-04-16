#include "is_integer.h"

#include <vector>

bool imas_partial::is_integer(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    char* end = nullptr;
    std::strtol(string.c_str(), &end, 10);
    return end != nullptr && *end == '\0';
}

bool imas_partial::is_range(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    char* end1 = nullptr;
    std::strtol(string.c_str(), &end1, 10);
    if (end1 == nullptr || *end1 != ':') {
        return false;
    }
    char* end2 = nullptr;
    std::strtol(end1 + 1, &end2, 10);
    return end2 != nullptr && *end2 == '\0';
}

imas_partial::Range imas_partial::parse_range(const std::string& string)
{
    std::vector<long> range;
    const char* end = string.c_str();
    while (end != nullptr && *end != '\0') {
        char* nend = nullptr;
        long num = std::strtol(end, &nend, 10);
        range.push_back(num);
        end = nend;
        if (*nend == ':') {
            ++end;
        }
    }
    if (range.size() == 2) {
        return Range(range[0], range[1]);
    } else if (range.size() == 3) {
        return Range(range[0], range[2], range[1]);
    } else {
        return Range(0, 0);
    }
}
