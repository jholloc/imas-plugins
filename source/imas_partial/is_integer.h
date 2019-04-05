#ifndef IMAS_PLUGINS_IS_INTEGER_H
#define IMAS_PLUGINS_IS_INTEGER_H

#include <string>

namespace imas_partial {

struct Range {
    long begin;
    long end;
    long stride;
    Range(long begin_, long end_, long stride_=1) : begin(begin_), end(end_), stride(stride_) {}
};

bool is_integer(const std::string& string);
bool is_range(const std::string& string);
Range parse_range(const std::string& string);

}

#endif //IMAS_PLUGINS_IS_INTEGER_H
