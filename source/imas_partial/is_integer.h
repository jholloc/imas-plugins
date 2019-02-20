#ifndef IMAS_PLUGINS_IS_INTEGER_H
#define IMAS_PLUGINS_IS_INTEGER_H

namespace imas_partial {

inline bool is_integer(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    char* end = nullptr;
    std::strtol(string.c_str(), &end, 10);
    return end != nullptr && *end == '\0';
}

}

#endif //IMAS_PLUGINS_IS_INTEGER_H
