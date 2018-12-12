#ifndef IMAS_PLUGINS_MACHINE_MAPPING_H
#define IMAS_PLUGINS_MACHINE_MAPPING_H

#include <string>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>

namespace uda {
namespace imas_mapping {

class MachineMapping {
public:
    MachineMapping() : mappings_{}
    {
        auto file_name = getenv("UDA_IMAS_MACHINE_MAP");
        if (file_name == nullptr) {
            throw std::runtime_error("environmental variable UDA_IMAS_MACHINE_MAP not set");
        }

        std::ifstream in_file(file_name);

        typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
        boost::char_separator<char> sep(" ");

        std::string line;
        while (std::getline(in_file, line)) {
            if (line[0] == '#') {
                continue;
            }

            tokenizer tokens{ line, sep };
            std::vector<std::string> words(tokens.begin(), tokens.end());
            if (words.size() != 4) {
                throw std::runtime_error(std::string("bad line in ") + file_name + ": " + line);
            }

            auto machine = words[0];
            auto ids = words[1];
            auto host = words[2];
            auto plugin = words[3];

            mappings_[machine][ids] = MappingValue{ host, plugin };
        }
    }

    std::string host(const std::string& machine, const std::string& ids)
    {
        auto& plugin_map = mappings_[machine];
        return plugin_map.count(ids) > 0 ? plugin_map[ids].host : plugin_map["*"].host;
    }

    std::string plugin(const std::string& machine, const std::string& ids)
    {
        auto& plugin_map = mappings_[machine];
        return plugin_map.count(ids) > 0 ? plugin_map[ids].plugin : plugin_map["*"].plugin;
    }

private:
    struct MappingValue {
        std::string host;
        std::string plugin;
    };

    std::unordered_map<std::string, std::unordered_map<std::string, MappingValue>> mappings_;
};

}
}

#endif // IMAS_PLUGINS_MACHINE_MAPPING_H
