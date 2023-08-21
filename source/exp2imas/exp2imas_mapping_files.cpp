#include "exp2imas_mapping_files.h"

#include <boost/filesystem.hpp>
#include <cstdlib>
#include <limits>
#include <sstream>

namespace bf = boost::filesystem;

// #include <logging/logging.h>

#define UDA_LOG_ERROR 0
#define UDA_LOG(LEVEL, FMT, ...) printf("%s:%d >> " FMT, bf::basename((char*)__FILE__).data(), __LINE__, ##__VA_ARGS__)

namespace {

unsigned int find_shot_range(const std::string& directory, int shot) {
    bf::path path{directory};

    long result = 0;

    for (const bf::directory_entry& item : bf::directory_iterator(path)) {
        if (bf::is_directory(item)) {
            std::string filename = item.path().filename().string();
            long num = std::strtol(filename.c_str(), nullptr, 10);
            if (num > shot) {
                break;
            }
            result = num;
        }
    }

    return static_cast<unsigned int>(result);
}

unsigned int find_version(const std::string& directory, const std::string& filename_prefix) {
    bf::path path{directory};

    long result = 0;

    for (const bf::directory_entry& item : bf::directory_iterator(path)) {
        if (bf::is_regular_file(item)) {
            std::string filename = item.path().filename().string();

            size_t pos = filename.find(filename_prefix + "_v");
            if (pos != std::string::npos) {
                long num = std::strtol(&filename[pos + filename_prefix.size() + 2], nullptr, 10);
                result = std::max(num, result);
            }
        }
    }

    return static_cast<unsigned int>(result);
}

struct Version {
    long major;
    long minor;
    long debug;
};

bool version_greater(const Version& lhs, const Version& rhs) {
    return lhs.major > rhs.major || (lhs.major == rhs.major && lhs.minor > rhs.minor) ||
           (lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.debug > rhs.debug);
}

std::string to_string(const Version& ver) {
    return std::to_string(ver.major) + "." + std::to_string(ver.minor) + "." + std::to_string(ver.debug);
}

Version find_imas_version(const std::string& directory, const std::string& filename_prefix,
                          const std::string& ids_version) {
    constexpr long long_max = std::numeric_limits<long>::max();
    Version version{long_max, long_max, long_max};
    if (!ids_version.empty()) {
        std::stringstream ss{ids_version};
        ss >> version.major >> version.minor >> version.debug;
    }

    bf::path path{directory};

    Version result{0, 0, 0};

    for (const bf::directory_entry& item : bf::directory_iterator(path)) {
        if (bf::is_regular_file(item)) {
            std::string filename = item.path().filename().string();

            size_t pos = filename.find(filename_prefix + "_v");
            if (pos != std::string::npos) {
                char* end = nullptr;
                Version found = {0, 0, 0};
                found.major = std::strtol(&filename[pos + filename_prefix.size() + 2], &end, 10);
                found.minor = end == nullptr ? 0 : std::strtol(end, &end, 10);
                found.debug = end == nullptr ? 0 : std::strtol(end, &end, 10);

                if (version_greater(found, version)) {
                    break;
                }

                result = found;
            }
        }
    }

    return result;
}

} // namespace

std::string uda::exp2imas::get_mapping_filename(const std::string& ids_version, const std::string& element) {
    static char* dir = nullptr;

    if (dir == nullptr) {
        dir = getenv("UDA_EXP2IMAS_MAPPING_FILE_DIRECTORY");
    }

    if (dir == nullptr) {
        UDA_LOG(UDA_LOG_ERROR, "environmental variable UDA_EXP2IMAS_MAPPING_FILE_DIRECTORY not set\n");
        return {};
    }

    size_t pos = element.find('/');
    std::string ids_name = element.substr(0, pos);

    std::string directory = std::string(dir) + "/IMAS";

    Version version = find_imas_version(directory, ids_name, ids_version);

    std::string name = directory + "/" + ids_name + "_v" + to_string(version) + ".xml";

    return name;
}

std::string uda::exp2imas::get_machine_mapping_filename(const std::string& experiment, const std::string& element,
                                                        int shot) {
    static char* dir = nullptr;

    if (dir == nullptr) {
        dir = getenv("UDA_EXP2IMAS_MAPPING_FILE_DIRECTORY");
    }

    if (dir == nullptr) {
        UDA_LOG(UDA_LOG_ERROR, "environmental variable UDA_EXP2IMAS_MAPPING_FILE_DIRECTORY not set\n");
        return {};
    }

    size_t pos = element.find('/');
    std::string ids_name = element.substr(0, pos);

    std::string directory = std::string(dir) + "/" + experiment + "/" + ids_name;

    unsigned int shot_range = find_shot_range(directory, shot);

    directory += "/" + std::to_string(shot_range);

    std::string filename = experiment + "_" + ids_name;

    unsigned int version = find_version(directory, filename);

    filename += "_v" + std::to_string(version) + ".xml";

    return directory + "/" + filename;
}
