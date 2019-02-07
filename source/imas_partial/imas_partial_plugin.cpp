#include "imas_partial_plugin.h"

#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <clientserver/copyStructs.h>
#include <clientserver/makeRequestBlock.h>
#include <plugins/udaPlugin.h>
//#include <plugins/pluginUtils.h>

#include "pugixml.hpp"

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
    int get(IDAM_PLUGIN_INTERFACE* plugin_interface);

private:
    pugi::xml_document doc_;
};

template<typename Out>
void split(const std::string& str, char delim, Out result) {
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> elems;
    split(str, delim, std::back_inserter(elems));
    return elems;
}

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
    const char* env = getenv("UDA_IMASDD_IDSDEF_PATH");
    if (env == nullptr) {
        throw std::runtime_error("UDA_IMASDD_IDSDEF_PATH environmental variable not set");
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
    std::vector<std::string> tokens = split(data_type, '_');
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
    std::vector<std::string> tokens = split(data_type, '_');
    if (tokens.back() == "type") {
        return 0;
    }
    return std::stoi(tokens.back());
}

void get_requests(std::vector<std::string>& requests, const std::string& ids, const std::string& path, const pugi::xml_node& node)
{
    std::string name = node.attribute("name").value();
    std::string dtype = node.attribute("data_type").value();

    std::stringstream out;

    if (dtype == "structure") {
        name = path + name + "/";
    } else if (dtype == "struct_array") {
        out << ids << "/" << path << name << "/Shape_of";
        requests.push_back(out.str()); 
        name = path + name + "/#/";
    } else { 
        out << ids << "/" << path << name;
        requests.push_back(out.str()); 
        name = path + name + "/";
    }

    for (const auto& child : node.children("field")) {
        get_requests(requests, ids, name, child);
    }
}

int call_plugin(const std::string& plugin_name, const std::string& request, IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK new_request{};
    copyRequestBlock(&new_request, *plugin_interface->request_block);

    strcpy(new_request.signal, request.c_str());

    makeRequestBlock(&new_request, *plugin_interface->pluginList, plugin_interface->environment);

    plugin_interface->request_block = &new_request;

    int plugin_id = findPluginIdByFormat(plugin_name.c_str(), plugin_interface->pluginList);
    if (plugin_id < 0) {
        RAISE_PLUGIN_ERROR("cannot find plugin");
    }

    PLUGIN_DATA* plugin = &plugin_interface->pluginList->plugin[plugin_id];
    int rc = plugin->idamPlugin(plugin_interface);
    if (rc < 0) {
        RAISE_PLUGIN_ERROR("call to plugin failed");
    }

    return 0;
}

void expand_request(std::vector<std::string>& requests, size_t depth, const std::string& request, const std::vector<int>& sizes)
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
    
} // anon namespace

int IMASPartialPlugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK* request_block = plugin_interface->request_block;

    const char* path = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    if (path == nullptr || path[0] == '\0') {
        RAISE_PLUGIN_ERROR("invalid path provided");
    }

    if (path[0] == '/') {
        path = &path[1];
    }

    std::vector<std::string> requests;

    std::vector<std::string> tokens = split(path, '/');

    if (!tokens.empty()) {
        std::string& ids = tokens[0];
     
        auto root = doc_.child("IDSs");

        auto nodes = root.find_child_by_attribute("IDS", "name", ids.c_str());
        for (const auto& node : nodes) {
            get_requests(requests, ids, "", node);
        }
    }

    std::vector<int> sizes;
    std::vector<std::string> total_requests;

    for (auto request : requests) {
        if (request.find("Shape_of") != std::string::npos) {
            auto depth = std::count(request.begin(), request.end(), '#');

            IDAM_PLUGIN_INTERFACE new_plugin_interface = *plugin_interface;
            DATA_BLOCK result{};
            new_plugin_interface.data_block = &result;
            int rc = call_plugin("imas_mapping", request, &new_plugin_interface);
            int size = 0;
            if (!rc) {
                size = *(int*)result.data;
            }

            if (depth < sizes.size()) {
                sizes.pop_back();
            }
            sizes.push_back(size);
        } else {
            expand_request(total_requests, 0, request, sizes);
        }
    }

    auto list = (DATA_BLOCK*)malloc(total_requests.size() * sizeof(DATA_BLOCK));

    int i = 0;
    for (const auto& request : total_requests) {
        //std::cout << request << std::endl;
        IDAM_PLUGIN_INTERFACE new_plugin_interface = *plugin_interface;

        DATA_BLOCK result{};
        new_plugin_interface.data_block = &result;

        call_plugin("imas_mapping", request, &new_plugin_interface);

        memcpy(&list[i], &result, sizeof(DATA_BLOCK));
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
        data_block->dims[i].dim_n = (int)total_requests.size();
        data_block->dims[i].compressed = 1;
        data_block->dims[i].dim0 = 0.0;
        data_block->dims[i].diff = 1.0;
        data_block->dims[i].method = 0;
    }

    data_block->data_type = UDA_TYPE_UNKNOWN;
    data_block->data = (char*)list;
    data_block->data_n = (int)total_requests.size();

    return 0;
}
