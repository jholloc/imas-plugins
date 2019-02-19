#include "imas_partial_plugin.h"

#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <deque>
#include <boost/algorithm/string.hpp>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <clientserver/copyStructs.h>
#include <clientserver/makeRequestBlock.h>
#include <plugins/udaPlugin.h>

#include "access_functions.h"
#include "pugixml.hpp"
#include "is_integer.h"

namespace {

class IMASPartialPlugin {
public:
    IMASPartialPlugin() = default;

    void init();
    void reset();

    int help(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int open(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int get(IDAM_PLUGIN_INTERFACE* plugin_interface);

private:
    pugi::xml_document doc_ = {};
    LLenv env_ = {};
};

} // anon namespace

int imasPartial(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static IMASPartialPlugin plugin;

        if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
            RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        }

        idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

        REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

        if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
            plugin.reset();
            return 0;
        }

        plugin.init();

        if (STR_IEQUALS(request_block->function, "init") || STR_IEQUALS(request_block->function, "initialise")) {
            return 0;
        }

        if (STR_IEQUALS(request_block->function, "help")) {
            return plugin.help(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "version")) {
            return plugin.version(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "builddate")) {
            return plugin.build_date(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "defaultmethod")) {
            return plugin.default_method(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "maxinterfaceversion")) {
            return plugin.max_interface_version(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "get")) {
            return plugin.get(idam_plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

void IMASPartialPlugin::init()
{
    const char* env = getenv("UDA_IMAS_PARTIAL_IDSDEF_PATH");
    if (env == nullptr) {
        throw std::runtime_error("UDA_IMAS_PARTIAL_IDSDEF_PATH environmental variable not set");
    }
    std::string dd_path = env;

    pugi::xml_parse_result result = doc_.load_file(dd_path.c_str());
    if (!result) {
        throw std::runtime_error(result.description());
    }
}

void IMASPartialPlugin::reset()
{

}

int IMASPartialPlugin::help(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    const char* help = "\ntemplatePlugin: Add Functions Names, Syntax, and Descriptions\n\n";
    const char* desc = "templatePlugin: help = description of this plugin";

    return setReturnDataString(plugin_interface->data_block, help, desc);
}

int IMASPartialPlugin::version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

int IMASPartialPlugin::build_date(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, __DATE__, "Plugin build date");
}

int IMASPartialPlugin::default_method(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

int IMASPartialPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}

namespace {

std::string get_type(const std::string& data_type)
{
    std::vector<std::string> tokens;
    boost::split(tokens, data_type, boost::is_any_of("_"), boost::token_compress_on);
    if (tokens[0] == "INT" || tokens[0] == "int") {
        return "integer";
    } else if (tokens[0] == "FLT" || tokens[0] == "flt") {
        return "float";
    } else if (tokens[0] == "STR") {
        return "string";
    }
    throw std::runtime_error("unknown type " + tokens[0]);
}

int get_rank(const std::string& data_type)
{
    std::vector<std::string> tokens;
    boost::split(tokens, data_type, boost::is_any_of("_"), boost::token_compress_on);
    if (tokens.back() == "type") {
        return 0;
    }
    return std::stoi(tokens.back());
}

void get_requests(LLenv& env, std::vector<std::string>& requests, std::string ids_path, const pugi::xml_node& node)
{
    std::string dtype = node.attribute("data_type").value();

    if (dtype == "struct_array") {
        int size = imas_partial::read_size_from_backend(env, ids_path);

        std::vector<std::string> tokens;
        boost::split(tokens, ids_path, boost::is_any_of("/"), boost::token_compress_on);

        if (imas_partial::is_integer(tokens.back())) {
            long val = strtol(tokens.back().c_str(), nullptr, 10);
            if (val > size) {
                throw std::range_error("out of range value given in ids_path");
            }
            for (const auto& child : node.children("field")) {
                get_requests(env, requests, ids_path + "/" + child.attribute("name").value(), child);
            }
        } else {
            for (int i = 1; i < size + 1; ++i) {
                std::string path = ids_path + "/" + std::to_string(i);
                for (const auto& child : node.children("field")) {
                    get_requests(env, requests, path + "/" + child.attribute("name").value(), child);
                }
            }
        }
    } else if (!dtype.empty() && dtype != "structure") {
        requests.push_back(ids_path);

        for (const auto& child : node.children("field")) {
            get_requests(env, requests, ids_path + "/" + child.attribute("name").value(), child);
        }
    } else {
        for (const auto& child : node.children("field")) {
            get_requests(env, requests, ids_path + "/" + child.attribute("name").value(), child);
        }
    }
}

void expand_request(std::vector<std::string>& requests, size_t depth, const std::string& request, const std::deque<int>& sizes)
{
    size_t pos = 0;
    if ((pos = request.find('#')) == std::string::npos) {
        requests.push_back(request);
        return;
    }
    if (depth > sizes.size()) {
        throw std::runtime_error("size not found");
    }
    for (int i = 0; i < sizes[depth]; ++i) {
        auto sub_request = request;
        sub_request.replace(pos, 1, std::to_string(i+1));
        expand_request(requests, depth + 1, sub_request, sizes);
    }
}

pugi::xml_node get_sibling_by_index(pugi::xml_node node, long index)
{
    for (long i = 1; i < index; ++i) {
        node = node.next_sibling();
    }
    return node;
}

int strtoi(const char* str, char** endptr, int base)
{
    long num = strtol(str, endptr, base);
    if (num > std::numeric_limits<int>::max()) {
        throw std::out_of_range{ std::to_string(num) + " out of range for reading as an integer" };
    }
    return static_cast<int>(num);
}

int imas_to_uda_type(int imas_type)
{
    switch (imas_type) {
        case CHAR_DATA: return UDA_TYPE_CHAR;
        case INTEGER_DATA: return UDA_TYPE_INT;
        case DOUBLE_DATA: return UDA_TYPE_DOUBLE;
        case COMPLEX_DATA: return UDA_TYPE_COMPLEX;
        default: return UDA_TYPE_UNKNOWN;
    }
}

LLenv open_pulse(IDAM_PLUGIN_INTERFACE* plugin_interface, int shot, int run, const char* user, const char* tokamak, const char* version)
{
    int ctxId = Lowlevel::beginPulseAction(ualconst::mdsplus_backend, shot, run, user, tokamak, version);
    LLenv env = Lowlevel::getLLenv(ctxId);

    int mode = ualconst::open_pulse;
    const char* options = "";

    auto pulseCtx = dynamic_cast<PulseContext*>(env.context);

    env.backend->openPulse(pulseCtx, mode, options);

    return env;
}

} // anon namespace

int IMASPartialPlugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK* request_block = plugin_interface->request_block;

    int shot;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, user);

    const char* tokamak;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, tokamak);

    const char* version;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, version);

    const char* path = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    if (env_.backend == nullptr) {
        env_ = open_pulse(plugin_interface, shot, run, user, tokamak, version);
    }

    if (path == nullptr || path[0] == '\0') {
        RAISE_PLUGIN_ERROR("invalid path provided");
    }

    if (path[0] == '/') {
        path = &path[1];
    }

    std::deque<std::string> tokens;
    boost::split(tokens, path, boost::is_any_of("/"), boost::token_compress_on);

    auto nodes = doc_.child("IDSs");
    std::string token;

//    std::deque<imas_partial::Range> ranges;
    std::string ids_path;
    std::string delim;

    while (!tokens.empty()) {
        token = tokens.front();

        if (imas_partial::is_integer(token)) {
            ids_path += delim + token;
        } else {
            nodes = nodes.find_child_by_attribute("name", token.c_str());
            ids_path += delim + token;
        }

        delim = "/";
        tokens.pop_front();
    }

    std::vector<std::string> requests;

    get_requests(env_, requests, ids_path, nodes);

    auto list = (DATA_BLOCK*)malloc(requests.size() * sizeof(DATA_BLOCK));

    int i = 0;
    for (const auto& request : requests) {
        initDataBlock(&list[i]);

        std::cout << request << std::endl;

        imas_partial::MDSData mds_data = imas_partial::read_data_from_backend(env_, request);

        DATA_BLOCK* data_block = &list[i];
        strcpy(data_block->data_label, request.c_str());

        data_block->rank = static_cast<unsigned int>(mds_data.rank);
        data_block->data = reinterpret_cast<char*>(mds_data.data);
        data_block->data_type = imas_to_uda_type(mds_data.datatype);

        if (mds_data.rank > 0 && mds_data.dims[0] == 0) {
            data_block->data_n = 0;
            data_block->rank = 0;
        } else if (mds_data.rank > 0) {
            data_block->data_n = 1;
            data_block->dims = (DIMS*)calloc(data_block->rank, sizeof(DIMS));

            for (int dim_i = 0; dim_i < data_block->rank; ++dim_i) {
                DIMS* dim = &data_block->dims[dim_i];
                initDimBlock(dim);
                dim->data_type = UDA_TYPE_UNSIGNED_INT;
                dim->dim_n = mds_data.dims[dim_i];
                dim->compressed = 1;
                dim->dim0 = 0;
                dim->diff = 1;
                dim->method = 0;
                data_block->data_n *= mds_data.dims[dim_i];
            }
        }

        ++i;
    }

    closeIdamError();

    DATA_BLOCK* data_block = plugin_interface->data_block;
    initDataBlock(data_block);

    int rank = 1;
    data_block->rank = (unsigned int)rank;
    data_block->dims = (DIMS*)malloc(rank * sizeof(DIMS));

    for (i = 0; i < rank; ++i) {
        initDimBlock(&data_block->dims[i]);

        data_block->dims[i].data_type = UDA_TYPE_UNSIGNED_INT;
        data_block->dims[i].dim_n = (int)requests.size();
        data_block->dims[i].compressed = 1;
        data_block->dims[i].dim0 = 0.0;
        data_block->dims[i].diff = 1.0;
        data_block->dims[i].method = 0;
    }

    data_block->data_type = UDA_TYPE_UNKNOWN;
    data_block->data = (char*)list;
    data_block->data_n = (int)(requests.size() * sizeof(DATA_BLOCK));

    return 0;
}
