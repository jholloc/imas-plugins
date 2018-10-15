#include "imas_mapping_plugin.h"
#include "machine_mapping.h"

#include <string>
#include <sstream>
#include <stack>
#include <vector>
#include <stack>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <regex.h>
#include <boost/format.hpp>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <clientserver/copyStructs.h>
#include <plugins/udaPlugin.h>
#if defined(UDA_VERSION) && UDA_VERSION_MAJOR > 2
#  include <plugins/pluginUtils.h>
#endif

namespace {

struct Pulse {
    const int shot;
    const int run;
    const std::string user;
    const std::string tokamak;
    const std::string version;

    Pulse(int shot_, int run_, std::string user_, std::string tokamak_, std::string version_)
            : shot(shot_), run(run_), user(std::move(user_)), tokamak(std::move(tokamak_)), version(std::move(version_))
    {}

    Pulse(const Pulse& other) = default;
};

class MappingPlugin {
public:
    MappingPlugin() = default;

    void init() {}
    void reset() {}

    int help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int open_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int close_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int begin_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int end_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int write_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int read_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int delete_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int begin_arraystruct_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

    int get(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int get_dim(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

private:
    uda::imas_mapping::MachineMapping machine_mapping_;
    std::string ids_;
    std::vector<std::pair<std::string, std::string>> arraystruct_stack_;
    std::unordered_map<int, Pulse> pulses_;
    int next_pulse_ = 0;
};

} // anon namespace

int imas_mapping_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static MappingPlugin plugin;

        if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
            RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        }

        idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

        REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

        std::string function = request_block->function;

        if (idam_plugin_interface->housekeeping || function == "reset") {
            plugin.reset();
            return 0;
        }

        plugin.init();

        if (function == "init" || function == "initialise") {
            return 0;
        }

        if (function == "help") {
            return plugin.help(idam_plugin_interface);
        } else if (function == "version") {
            return plugin.version(idam_plugin_interface);
        } else if (function == "builddate") {
            return plugin.build_date(idam_plugin_interface);
        } else if (function == "defaultmethod") {
            return plugin.default_method(idam_plugin_interface);
        } else if (function == "maxinterfaceversion") {
            return plugin.max_interface_version(idam_plugin_interface);
        } else if (function == "openPulse") {
            return plugin.open_pulse(idam_plugin_interface);
        } else if (function == "closePulse") {
            return plugin.close_pulse(idam_plugin_interface);
        } else if (function == "beginAction") {
            return plugin.begin_action(idam_plugin_interface);
        } else if (function == "endAction") {
            return plugin.end_action(idam_plugin_interface);
        } else if (function == "writeData") {
            return plugin.write_data(idam_plugin_interface);
        } else if (function == "readData") {
            return plugin.read_data(idam_plugin_interface);
        } else if (function == "deleteData") {
            return plugin.delete_data(idam_plugin_interface);
        } else if (function == "beginArraystructAction") {
            return plugin.begin_arraystruct_action(idam_plugin_interface);
        } else if (function == "get") {
            return plugin.get(idam_plugin_interface);
        } else if (function == "getDim") {
            return plugin.get_dim(idam_plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

namespace {

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int MappingPlugin::help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* help = "\ntemplatePlugin: Add Functions Names, Syntax, and Descriptions\n\n";
    const char* desc = "templatePlugin: help = description of this plugin";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

/**
 * Plugin version
 * @param idam_plugin_interface
 * @return
 */
int MappingPlugin::version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int MappingPlugin::build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int MappingPlugin::default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int MappingPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

int MappingPlugin::open_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

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

    pulses_.emplace(next_pulse_, Pulse{ shot, run, user, tokamak, version });

    setReturnDataIntScalar(idam_plugin_interface->data_block, next_pulse_, nullptr);

    ++next_pulse_;
    return 0;
}

int MappingPlugin::close_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    pulses_.erase(ctxId);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int MappingPlugin::begin_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    const char* dataObject;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, dataObject);

    int access;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, access);

    int range;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, range);

#ifdef FIND_REQUIRED_DOUBLE_VALUE
    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);
#else
    float time;
    FIND_REQUIRED_FLOAT_VALUE(request_block->nameValueList, time);
#endif

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    if (pulses_.count(ctxId) == 0) {
        RAISE_PLUGIN_ERROR("invalid ctxId");
    }

    ids_ = dataObject;

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int MappingPlugin::end_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    int type;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, type);

    if (pulses_.count(ctxId) == 0) {
        RAISE_PLUGIN_ERROR("invalid ctxId");
    }

    if (arraystruct_stack_.empty()) {
        ids_ = "";
    } else {
        arraystruct_stack_.pop_back();
    }

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int MappingPlugin::write_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    RAISE_PLUGIN_ERROR("cannot write data with mapping plugin");
}

int MappingPlugin::delete_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    RAISE_PLUGIN_ERROR("cannot delete data with mapping plugin");
}

bool is_integer(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    char* end = nullptr;
    std::strtol(string.c_str(), &end, 10);
    return end != nullptr && *end == '\0';
}

int MappingPlugin::begin_arraystruct_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    int size;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, size);

    const char* dataObject;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, dataObject);

    int access;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, access);

    int range;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, range);

#ifdef FIND_REQUIRED_DOUBLE_VALUE
    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);
#else
    float time;
    FIND_REQUIRED_FLOAT_VALUE(request_block->nameValueList, time);
#endif

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    const char* path;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    const char* timebase;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, timebase);

    if (pulses_.count(ctxId) == 0) {
        RAISE_PLUGIN_ERROR("invalid ctxId");
    }

    Pulse pulse = pulses_.find(ctxId)->second;

    std::vector<std::string> tokens;

    std::stringstream ss(path);
    std::string fragment;
    while (std::getline(ss, fragment, '/')) {
        tokens.push_back(fragment);
    }

    auto plugin_name = machine_mapping_.plugin(pulse.tokamak);

    int uda_type = UDA_TYPE_INT;

    std::stringstream element_ss;
    std::stringstream indices_ss;

    const char* element_delim = "";
    const char* indices_delim = "";

    for (const auto& el : tokens) {
        if (is_integer(el)) {
            element_ss << element_delim << "#";
            indices_ss << indices_delim << el;
            indices_delim = ";";
        } else {
            element_ss << element_delim << el;
        }
        element_delim = "/";
    }

    arraystruct_stack_.emplace_back(element_ss.str(), indices_ss.str());

    auto request =
            boost::format("%s::read(experiment='%s', element='%s/%s/Shape_of', shot=%d, indices='%s', dtype=%d, IDS_version='')")
            % plugin_name % pulse.tokamak % ids_ % element_ss.str() % pulse.shot % indices_ss.str() % uda_type;
    std::cout << request.str() << std::endl;

    return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
}

size_t extract_array_indices(const char* input, char** output, int** indices)
{
    regex_t preg;
    int rc;
    const char* pattern = "(/[0-9]+/)";

    rc = regcomp(&preg, pattern, REG_EXTENDED);
    if (rc != 0) {
        fprintf(stderr, "regcomp() failed\n");
        return (size_t)rc;
    }

    char* work = strdup(input);
    *indices = nullptr;
    size_t num_indices = 0;

    size_t nmatch = 1;
    regmatch_t pmatch[1];
    while (regexec(&preg, work, nmatch, pmatch, 0) == 0) {
        size_t len = pmatch[0].rm_so + (strlen(input) - pmatch[0].rm_eo) + 4;

        auto temp = (char*)calloc(len, sizeof(char));
        strncpy(temp, work, (size_t)pmatch[0].rm_so);
        strcpy(temp + pmatch[0].rm_so, "/#/");
        strcpy(temp + pmatch[0].rm_so + 3, &work[pmatch[0].rm_eo]);

        auto index = (char*)calloc((size_t)(pmatch[0].rm_eo - pmatch[0].rm_so - 1), sizeof(char));
        strncpy(index, &work[pmatch[0].rm_so + 1], (size_t)(pmatch[0].rm_eo - pmatch[0].rm_so - 2));
        *indices = (int*)realloc(*indices, (num_indices + 1) * sizeof(int));
        (*indices)[num_indices] = (int)strtol(index, nullptr, 10);
        ++num_indices;

        free(work);
        work = temp;
    }

    *output = work;

    return num_indices;
}

char* indices_to_string(const int* indices, size_t num_indices)
{
    if (num_indices == 0) return strdup("-1");

    char* string = strdup("");

    int i;
    for (i = 0; i < num_indices; ++i) {
        auto len = (size_t)snprintf(nullptr, 0, "%s%d;", string, indices[i]);
        auto temp = (char*)malloc(len + 1);
        snprintf(temp, len + 1, "%s%d;", string, indices[i]);
        free(string);
        string = temp;
    }

    string[strlen(string) - 1] = '\0'; // remove last ';'
    return string;
}

int convert_IMAS_to_UDA_type(const std::string& type)
{
    if (type == "integer") {
        return UDA_TYPE_INT;
    } else if (type == "float") {
        return UDA_TYPE_FLOAT;
    } else if (type == "double") {
        return UDA_TYPE_DOUBLE;
    } else if (type == "string") {
        return UDA_TYPE_STRING;
    } else {
        return UDA_TYPE_UNKNOWN;
    }
}

int convert_IMAS_to_UDA_type(int type)
{
    if (type == 50) {
        return UDA_TYPE_STRING;
    } else if (type == 51) {
        return UDA_TYPE_INT;
    } else if (type == 52) {
        return UDA_TYPE_DOUBLE;
    } else {
        return UDA_TYPE_UNKNOWN;
    }
}

int MappingPlugin::read_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    const char* field;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, field);

    const char* timebase;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, timebase);

    int datatype;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, datatype);

    int index;
    bool is_index = FIND_INT_VALUE(request_block->nameValueList, index);

    if (!arraystruct_stack_.empty() && !is_index) {
        RAISE_PLUGIN_ERROR("no index given while reading an arraystruct field");
    }

    if (pulses_.count(ctxId) == 0) {
        RAISE_PLUGIN_ERROR("invalid ctxId");
    }

    Pulse pulse = pulses_.find(ctxId)->second;

    auto plugin_name = machine_mapping_.plugin(pulse.tokamak);

    std::string element;
    std::string indices;

    if (arraystruct_stack_.empty()) {
        element = field;
    } else {
        element = arraystruct_stack_.back().first + "/#/" + field;
        if (arraystruct_stack_.back().second.empty()) {
            indices = std::to_string(index);
        } else {
            indices = arraystruct_stack_.back().second + ";" + std::to_string(index);
        }
    }

    int uda_type = convert_IMAS_to_UDA_type(datatype);

    auto request =
            boost::format("%s::read(experiment='%s', element='%s/%s', shot=%d, indices='%s', dtype=%d, IDS_version='')")
            % plugin_name % pulse.tokamak % ids_ % element % pulse.shot % indices % uda_type;
    std::cout << request.str() << std::endl;

    return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
}

int MappingPlugin::get(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* expName;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, expName);

    const char* group;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, group);

    const char* type;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, type);

    const char* variable;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, variable);

    int shot;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, user);

    auto plugin_name = machine_mapping_.plugin(expName);

    auto element = std::string(group) + "/" + variable;

    char* path = nullptr;
    int* indices = nullptr;
    size_t num_indices = extract_array_indices(element.c_str(), &path, &indices);
    char* indices_string = indices_to_string(indices, num_indices);

    int uda_type = convert_IMAS_to_UDA_type(type);

    auto request =
            boost::format("%s::read(experiment='%s', element='%s', shot=%d, indices='%s', dtype=%d, IDS_version='')")
            % plugin_name % expName % path % shot % indices_string % uda_type;
    std::cout << request.str() << std::endl;

    return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
}

int MappingPlugin::get_dim(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* expName;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, expName);

    const char* group;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, group);

    const char* variable;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, variable);

    int shot;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, user);

    auto plugin_name = machine_mapping_.plugin(expName);

    auto element = std::string(group) + "/" + variable + "/Shape_of";

    char* path = nullptr;
    int* indices = nullptr;
    size_t num_indices = extract_array_indices(element.c_str(), &path, &indices);
    char* indices_string = indices_to_string(indices, num_indices);

    int uda_type = UDA_TYPE_INT;

    auto request =
            boost::format("%s::read(experiment='%s', element='%s', shot=%d, indices='%s', dtype=%d, IDS_version='')")
            % plugin_name % expName % path % shot % indices_string % uda_type;
    std::cout << request.str() << std::endl;

    return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
}

} // anon namespace
