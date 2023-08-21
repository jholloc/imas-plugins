#ifndef IMAS_PLUGINS_MACHINE_MAPPING_H
#define IMAS_PLUGINS_MACHINE_MAPPING_H

#include <string>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

namespace uda {
namespace plugins {
namespace imas {

constexpr size_t MappingFileColumnCount = 5;

class MachineMapping
{
public:
    MachineMapping()
    {
        auto *file_name = getenv("UDA_IMAS_MACHINE_MAP");
        if (file_name == nullptr) {
            throw std::runtime_error("environmental variable UDA_IMAS_MACHINE_MAP not set");
        }

        std::ifstream in_file(file_name);

        std::string line;
        while (std::getline(in_file, line)) {
            if (line[0] == '#') {
                continue;
            }

            std::vector<std::string> words;
            boost::split(words, line, boost::is_any_of(" \t"), boost::token_compress_on);
            if (words.size() != MappingFileColumnCount) {
                throw std::runtime_error(std::string("bad line in ") + file_name + ": " + line);
            }

            auto machine = words[0];
            auto ids = words[1];
            auto host = words[2] == "-" ? "localhost" : words[2];
            auto port = words[3] == "-" ? 0 : std::stoi(words[3]);
            auto plugin = words[4];

            mappings_[machine][ids] = MappingValue{host, port, plugin};
        }
    }

    bool contains(std::string machine)
    {
        boost::to_upper(machine);
        return mappings_.count(machine) != 0;
    }

    std::string host(std::string machine, std::string ids)
    {
        boost::to_upper(machine);
        boost::to_upper(ids);
        auto& plugin_map = mappings_[machine];
        return plugin_map.count(ids) > 0 ? plugin_map[ids].host : plugin_map["*"].host;
    }

    int port(std::string machine, std::string ids)
    {
        boost::to_upper(machine);
        boost::to_upper(ids);
        auto& plugin_map = mappings_[machine];
        return plugin_map.count(ids) > 0 ? plugin_map[ids].port : plugin_map["*"].port;
    }

    std::string plugin(std::string machine, std::string ids)
    {
        boost::to_upper(machine);
        boost::to_upper(ids);
        auto& plugin_map = mappings_[machine];
        return plugin_map.count(ids) > 0 ? plugin_map[ids].plugin : plugin_map["*"].plugin;
    }

private:
    struct MappingValue
    {
        std::string host;
        int port;
        std::string plugin;
    };

    std::unordered_map<std::string, std::unordered_map<std::string, MappingValue>> mappings_;
};

}
}
}

#endif // IMAS_PLUGINS_MACHINE_MAPPING_H
