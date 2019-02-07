#ifndef IMAS_PARTIAL_PLUGIN_ACCESS_FUNCTIONS_H
#define IMAS_PARTIAL_PLUGIN_ACCESS_FUNCTIONS_H

#include <string>
#include <deque>

#include <ual_lowlevel.h>

namespace imas_partial {

int read_size_from_backend(LLenv& env, const std::string& str, std::deque<int> indices);

void* read_data_from_backend(LLenv& env, const std::string& str, std::deque<int> indices);

}

#endif // IMAS_PARTIAL_PLUGIN_ACCESS_FUNCTIONS_H
