#ifndef IMAS_PARTIAL_PLUGIN_ACCESS_FUNCTIONS_H
#define IMAS_PARTIAL_PLUGIN_ACCESS_FUNCTIONS_H

#include <string>
#include <deque>

#include <ual_lowlevel.h>

namespace imas_partial {

struct Range {
    Range(int b, int e) : begin{b}, end{e} {}
    int begin;
    int end;
};

struct MDSData {
    void* data = nullptr;
    int rank = 0;
    int dims[64] = {0};
    int datatype = 0;
};

int read_size_from_backend(LLenv& env, const std::string& str);

MDSData read_data_from_backend(LLenv& env, const std::string& str);

}

#endif // IMAS_PARTIAL_PLUGIN_ACCESS_FUNCTIONS_H
