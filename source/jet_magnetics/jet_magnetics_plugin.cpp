#include "jet_magnetics_plugin.h"

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include <plugins/udaPlugin.h>
#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>

#include "tinyxml2.h"

namespace {

struct FluxLoop {
    long num;
    float r[2];
    float z[2];
    float factor;
    float rel_err;
    float abs_err;
    std::string name;
    std::string desc;
    long oct;
    std::string magn;
    std::string dda;
    std::string dtype;
    std::string source;
};

struct PickUp {
    long num;
    float r;
    float z;
    float pol_ang;
    float factor;
    float rel_err;
    float abs_err;
    std::string name;
    std::string desc;
    long oct;
    float tor_ang;
    std::string magn;
    std::string dda;
    std::string dtype;
};

enum class Section
{
    NONE,
    FLUX,
    SADDLE,
    PICK_UP,
    HALL,
    OTHER,
};

class Filter {
public:
    explicit Filter(tinyxml2::XMLElement* element)
    {
        if (element == nullptr) {
            throw std::runtime_error("null element");
        }
        _dda = element->Attribute("dda") != nullptr ? element->Attribute("dda") : "";
        _dtype = element->Attribute("dtype") != nullptr ? element->Attribute("dtype") : "";
        _sorted = element->Attribute("sorted") != nullptr ? element->Attribute("sorted") : "";
    }

    template <typename T>
    std::vector<T> insert_matches(std::vector<T>& filtered, const std::vector<T>& items) const {
        std::copy_if(items.begin(), items.end(), std::back_inserter(filtered), [this](const T& loop){ return match(loop); });

        if (_sorted == "dtype") {
            std::sort(filtered.begin(), filtered.end(), [](const T& lhs, const T& rhs){ return lhs.dtype.compare(rhs.dtype); });
        }

        return filtered;
    }

private:
    std::string _dda;
    std::string _dtype;
    std::string _sorted;

    bool match(const FluxLoop& loop) const
    {
        return match(loop.dda, loop.dtype);
    }

    bool match(const PickUp& pickup) const
    {
        return match(pickup.dda, pickup.dtype);
    }

    bool match(const std::string& dda, const std::string& dtype) const
    {
        if (!_dda.empty() && dda != _dda) {
            return false;
        }

        if (!_dtype.empty()) {
            if (_dtype[0] == '!') {
                if (dtype ==  _dtype.substr(1)) {
                    return false;
                }
            } else {
                if (dtype != _dtype) {
                    return false;
                }
            }
        }

        return true;
    }

};

class JetMagneticsPlugin
{
public:
    void reset() {}
    void init() {
        load_filters();
        load_data();
        filter_data();
    }

    int help(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);
    int read(IDAM_PLUGIN_INTERFACE * idam_plugin_interface, const JetMagneticsPlugin& plugin);

private:
    std::vector<Filter> _flux_loop_filters;
    std::vector<Filter> _bpol_probe_filters;
    std::vector<FluxLoop> _flux_loops;
    std::vector<PickUp> _pickups;

    void filter_data()
    {
        std::vector<FluxLoop> filtered_flux_loops;
        std::vector<PickUp> filtered_pickups;

        for (const auto& filter : _flux_loop_filters) {
            auto matches = filter.insert_matches(filtered_flux_loops, _flux_loops);
        }

        for (const auto& filter : _bpol_probe_filters) {
            auto matches = filter.insert_matches(filtered_pickups, _pickups);
        }

        _flux_loops = filtered_flux_loops;
        _pickups = filtered_pickups;
    }

    void load_filters()
    {
        tinyxml2::XMLDocument document;
        document.LoadFile("/Users/jhollocombe/Projects/iter-plugins/source/jet_magnetics/sensors.xml");

        tinyxml2::XMLNode* root = document.FirstChildElement("filters")->FirstChildElement("flux_loops");

        for (tinyxml2::XMLNode* child = root->FirstChild(); child != root->LastChild(); child->NextSibling()) {
            auto el = dynamic_cast<tinyxml2::XMLElement*>(child);
            _flux_loop_filters.emplace_back(Filter{ el });
        }

        root = document.FirstChildElement("filters")->FirstChildElement("bpol_probes");

        for (tinyxml2::XMLNode* child = root->FirstChild(); child != root->LastChild(); child->NextSibling()) {
            auto el = dynamic_cast<tinyxml2::XMLElement*>(child);
            _bpol_probe_filters.emplace_back(Filter{ el });
        }
    }

    void load_data()
    {

        std::ifstream infile("/Users/jhollocombe/Projects/iter-plugins/source/jet_magnetics/sensors_200c_detail.txt");

        Section section = Section::NONE;

        std::string line;
        while (std::getline(infile, line))
        {
            boost::trim(line);

            if (line.empty()) {
                continue;
            }

            if (boost::starts_with(line, "#END")) {
                section = Section::NONE;
            } else if (boost::starts_with(line, "#FLUX")) {
                section = Section::FLUX;
                continue;
            } else if (boost::starts_with(line, "#SADDLE")) {
                section = Section::SADDLE;
                continue;
            } else if (boost::starts_with(line, "#PICK-UP")) {
                section = Section::PICK_UP;
                continue;
            } else if (boost::starts_with(line, "#HALL")) {
                section = Section::HALL;
                continue;
            } else if (boost::starts_with(line, "#OTHER")) {
                section = Section::OTHER;
                continue;
            }

            std::vector<std::string> tokens;
            boost::split(tokens, line, boost::is_any_of(","), boost::token_compress_on);

            if (section == Section::FLUX) {
                int i = 0;
                auto num = strtol(tokens[i++].c_str(), nullptr, 10);
                auto r = strtof(tokens[i++].c_str(), nullptr);
                auto z = strtof(tokens[i++].c_str(), nullptr);
                auto factor = strtof(tokens[i++].c_str(), nullptr);
                auto rel_err = strtof(tokens[i++].c_str(), nullptr);
                auto abs_err = strtof(tokens[i++].c_str(), nullptr);
                auto name = tokens[i++];
                auto desc = tokens[i++];
                auto oct = strtol(tokens[i++].c_str(), nullptr, 10);
                auto magn = tokens[i++];
                auto dda = tokens[i++];
                auto dtype = tokens[i++];
                auto source = std::string("PPF/MAGN/") + magn;
                _flux_loops.emplace_back(FluxLoop{ num, { r, 0 }, { z, 0 }, factor, rel_err, abs_err, name, desc, oct, magn, dda, dtype, source });
            } else if (section == Section::SADDLE) {
                int i = 0;
                auto num = strtol(tokens[i++].c_str(), nullptr, 10);
                auto r1 = strtof(tokens[i++].c_str(), nullptr);
                auto r2 = strtof(tokens[i++].c_str(), nullptr);
                auto z1 = strtof(tokens[i++].c_str(), nullptr);
                auto z2 = strtof(tokens[i++].c_str(), nullptr);
                auto factor = strtof(tokens[i++].c_str(), nullptr);
                auto rel_err = strtof(tokens[i++].c_str(), nullptr);
                auto abs_err = strtof(tokens[i++].c_str(), nullptr);
                auto name = tokens[i++];
                auto desc = tokens[i++];
                auto oct = strtol(tokens[i++].c_str(), nullptr, 10);
                auto magn = tokens[i++];
                auto dda = tokens[i++];
                auto dtype = tokens[i++];
                auto source = std::string("PPF/MAGN/") + magn;
                _flux_loops.emplace_back(FluxLoop{ num, { r1, r2 }, { z1, z2 }, factor, rel_err, abs_err, name, desc, oct, magn, dda, dtype, source });
            } else if (section == Section::PICK_UP) {
                int i = 0;
                auto num = strtol(tokens[i++].c_str(), nullptr, 10);
                auto r = strtof(tokens[i++].c_str(), nullptr);
                auto z = strtof(tokens[i++].c_str(), nullptr);
                auto pol_ang = strtof(tokens[i++].c_str(), nullptr);
                auto factor = strtof(tokens[i++].c_str(), nullptr);
                auto rel_err = strtof(tokens[i++].c_str(), nullptr);
                auto abs_err = strtof(tokens[i++].c_str(), nullptr);
                auto name = tokens[i++];
                auto desc = tokens[i++];
                auto oct = strtol(tokens[i++].c_str(), nullptr, 10);
                auto tor_ang = strtof(tokens[i++].c_str(), nullptr);
                auto magn = tokens[i++];
                auto dda = tokens[i++];
                auto dtype = tokens[i++];
                _pickups.emplace_back(PickUp{ num, r, z, pol_ang, factor, rel_err, abs_err, name, desc, oct, tor_ang, magn, dda, dtype });
            }
        }
    }
};

} // anon namespace

int jetMagneticsPlugin(IDAM_PLUGIN_INTERFACE * idam_plugin_interface)
{
    if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;
    int err = 0;

    try {

        static JetMagneticsPlugin plugin;

        static bool init = false;

        if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
            plugin.reset();
            return 0;
        }

        if (!init || STR_IEQUALS(request_block->function, "init") ||
            STR_IEQUALS(request_block->function, "initialise")) {
            plugin.init();
            if (STR_IEQUALS(request_block->function, "init") || STR_IEQUALS(request_block->function, "initialise")) {
                return 0;
            }
        }

        if (STR_IEQUALS(request_block->function, "help")) {
            err = plugin.help(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "version")) {
            err = plugin.version(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "builddate")) {
            err = plugin.build_date(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "defaultmethod")) {
            err = plugin.default_method(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "maxinterfaceversion")) {
            err = plugin.max_interface_version(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "read")) {
            err = plugin.read(idam_plugin_interface, plugin);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }

    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }

    return err;
}

namespace {

// Help: A Description of library functionality
int JetMagneticsPlugin::help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* help = "\ntsPlugin: this plugin maps Tore Supra data to IDS\n\n";
    const char* desc = "tsPlugin: help = plugin used for mapping Tore Supra experimental data to IDS";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

int JetMagneticsPlugin::version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin version number";

    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, desc);
}

// Plugin Build Date
int JetMagneticsPlugin::build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin build date";

    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, desc);
}

// Plugin Default Method
int JetMagneticsPlugin::default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin default method";

    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, desc);
}

// Plugin Maximum Interface Version
int JetMagneticsPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Maximum Interface Version";

    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, desc);
}

int JetMagneticsPlugin::read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const JetMagneticsPlugin& plugin)
{
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    initDataBlock(data_block);

    data_block->rank = 0;
    data_block->dims = nullptr;

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* element = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);

    int shot = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int* indices = nullptr;
    size_t nindices = 0;
    FIND_REQUIRED_INT_ARRAY(request_block->nameValueList, indices);

    if (nindices == 1 && indices[0] == -1) {
        nindices = 0;
        free(indices);
        indices = nullptr;
    }

    int dtype = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, dtype);

    const char* IDS_version = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, IDS_version);

    const char* experiment = nullptr;
    FIND_STRING_VALUE(request_block->nameValueList, experiment);

    if (std::string{ experiment } != "JET") {
        RAISE_PLUGIN_ERROR("wrong experiment for plugin JET_MAGNETICS");
    }

    // magnetics/bpol_probe/#/name

    std::vector<std::string> tokens;
    boost::split(tokens, element, boost::is_any_of("/"), boost::token_compress_on);

    if (tokens[0] != "magnetics") {
        RAISE_PLUGIN_ERROR("wrong IDS for plugin JET_MAGNETICS");
    }

    if (tokens[1] == "flux_loop") {
        if (tokens[2] == "Shape_of") {
            setReturnDataIntScalar(data_block, (int)_flux_loops.size(), nullptr);
            return 0;
        } else if (tokens[2] == "#") {
            long index = indices[0];
            const FluxLoop& flux_loop = _flux_loops[index - 1];
            if (tokens[3] == "name") {
                setReturnDataString(data_block, flux_loop.name.c_str(), nullptr);
                return 0;
            } else if (tokens[3] == "identifier") {
                setReturnDataIntScalar(data_block, (int)flux_loop.num, nullptr);
                return 0;
            } else if (tokens[3] == "position") {
                if (tokens[4] == "Shape_of") {
                    setReturnDataIntScalar(data_block, 2, nullptr);
                    return 0;
                } else if (tokens[4] == "#") {
                    long pos_index = indices[1];
                    if (tokens[5] == "r") {
                        setReturnDataFloatScalar(data_block, flux_loop.r[pos_index], nullptr);
                        return 0;
                    } else if (tokens[5] == "z") {
                        setReturnDataFloatScalar(data_block, flux_loop.z[pos_index], nullptr);
                        return 0;
                    } else if (tokens[5] == "phi") {
                        setReturnDataFloatScalar(data_block, 0.0, nullptr);
                        return 0;
                    }
                } else if (tokens[3] == "flux") {
                    if (tokens[4] == "data") {
                        return 1;
                    } else if (tokens[4] == "data_error_upper") {
                        return 1;
                    } else if (tokens[4] == "data_error_lower") {
                        return 1;
                    } else if (tokens[4] == "time") {
                        return 1;
                    }
                }
            }
        }
    } else if (tokens[1] == "bpol_probe") {
        if (tokens[2] == "Shape_of") {
            setReturnDataIntScalar(data_block, (int)_pickups.size(), nullptr);
            return 0;
        } else if (tokens[2] == "#") {
            long index = indices[0];
            const PickUp& pick_up = _pickups[index - 1];
            if (tokens[3] == "name") {
                setReturnDataString(data_block, pick_up.name.c_str(), nullptr);
                return 0;
            } else if (tokens[3] == "identifier") {
                setReturnDataIntScalar(data_block, (int)pick_up.num, nullptr);
                return 0;
            } else if (tokens[3] == "toroidal_angle") {
                setReturnDataFloatScalar(data_block, pick_up.tor_ang, nullptr);
                return 0;
            } else if (tokens[3] == "poloidal_angle") {
                setReturnDataFloatScalar(data_block, pick_up.pol_ang, nullptr);
                return 0;
            } else if (tokens[3] == "area") {
                setReturnDataFloatScalar(data_block, 0.001, nullptr);
                return 0;
            } else if (tokens[3] == "position") {
                if (tokens[4] == "r") {
                    setReturnDataFloatScalar(data_block, pick_up.r, nullptr);
                    return 0;
                } else if (tokens[4] == "z") {
                    setReturnDataFloatScalar(data_block, pick_up.z, nullptr);
                    return 0;
                }
            } else if (tokens[3] == "length") {
                return 1;
            } else if (tokens[3] == "turns") {
                return 1;
            } else if (tokens[3] == "field") {
                if (tokens[4] == "data") {
                    return 1;
                } else if (tokens[4] == "data_error_upper") {
                    return 1;
                } else if (tokens[4] == "data_error_lower") {
                    return 1;
                } else if (tokens[4] == "time") {
                    return 1;
                }
            }
        }
    } else {
        RAISE_PLUGIN_ERROR("unknown IDS element");
    }

    return 1;
}

} // anon namespace