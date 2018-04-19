#ifndef IMAS_PLUGINS_MACHINE_MAPPING_H
#define IMAS_PLUGINS_MACHINE_MAPPING_H

#include <string>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>

namespace {

template <typename T>
struct irange {
    explicit irange(std::istream& in) : d_in(in)
    {}

    std::istream& d_in;
};

template <typename T>
std::istream_iterator<T> begin(irange<T> r)
{
    return std::istream_iterator<T>(r.d_in);
}

template <typename T>
std::istream_iterator<T> end(irange<T>)
{
    return std::istream_iterator<T>();
}

}

namespace imas_uda_plugins {

class MachineMapping {
public:
    MachineMapping() : mappings_{}
    {
        auto file_name = getenv("UDA_IMAS_MACHINE_MAP");
        if (file_name == nullptr) {
            throw std::runtime_error("environmental variable UDA_IMAS_MACHINE_MAP not set");
        }

        std::ifstream in_file(file_name);
        for (const auto& line : irange<std::string>(in_file)) {
            boost::tokenizer<> tokens{ line };
            std::vector<std::string> words(tokens.begin(), tokens.end());
            if (words.size() != 3) {
                throw std::runtime_error(std::string("bad line in ") + file_name + ": " + line);
            }

            auto machine = words[0];
            auto host = words[1];
            auto plugin = words[2];

            mappings_[machine] = MappingValue{ host, plugin };
        }
    }

    std::string host(const std::string& machine) {
        return mappings_[machine].host;
    }

    std::string plugin(const std::string& machine) {
        return mappings_[machine].plugin;
    }

private:
    struct MappingValue {
        std::string host;
        std::string plugin;
    };

    std::unordered_map<std::string, MappingValue> mappings_;
};

}

#endif // IMAS_PLUGINS_MACHINE_MAPPING_H
