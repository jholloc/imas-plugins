#ifndef IMAS_PLUGINS_MACHINE_MAPPING_H
#define IMAS_PLUGINS_MACHINE_MAPPING_H

#include <string>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

namespace iter {
namespace md {

class ConfigMapping {
public:
    ConfigMapping() : mappings_{}
    {
        auto file_name = getenv("UDA_IMAS_CONFIG_MAP");
        if (file_name == nullptr) {
            throw std::runtime_error("environmental variable UDA_IMAS_CONFIG_MAP not set");
        }

        std::ifstream in_file(file_name);

        typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
        boost::char_separator<char> sep(" ");

        std::string line;
        while (std::getline(in_file, line)) {
            boost::trim(line);
            if (line[0] == '#') {
                continue;
            }

            std::vector<std::string> words;
            boost::split(words, line, boost::is_any_of(" \t"), boost::token_compress_on);
            if (words.size() != 3) {
                throw std::runtime_error(std::string("bad line in ") + file_name + ": " + line);
            }

            auto shot_str = words[0];
            auto machine_version = words[1];
            auto config_name = words[2];

            size_t pos;
            int shot = std::stoi(shot_str, &pos);
            if (pos != shot_str.size()) {
                throw std::runtime_error(std::string("bad shot number given in line ") + line);
            }

            mappings_[shot] = MappingData{ machine_version, config_name };
        }
    }

    std::string config_name(int shot)
    {
        return mappings_.count(shot) ? mappings_[shot].config_name : "";
    }

    std::string machine_version(int shot)
    {
        return mappings_.count(shot) ? mappings_[shot].machine_version : "";
    }

private:
    struct MappingData {
        std::string machine_version;
        std::string config_name;
    };

    std::unordered_map<int, MappingData> mappings_;
};

}
}

#endif // IMAS_PLUGINS_MACHINE_MAPPING_H
