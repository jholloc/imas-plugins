#include "imas_mapping_plugin.h"
#include "machine_mapping.h"

#include <string>
#include <sstream>
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
#include <plugins/serverPlugin.h>
#include <server/makeServerRequestBlock.h>

class imas_uda_plugins::MappingPlugin {
public:
    imas_uda_plugins::MappingPlugin() = default;

    void init() {}
    void reset() {}

    int help(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int open(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int close(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int get(IDAM_PLUGIN_INTERFACE* plugin_interface);

private:
    MachineMapping machine_mapping_;
};

int imas_mapping_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static imas_uda_plugins::MappingPlugin plugin;

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
        } else if (STR_IEQUALS(request_block->function, "open")) {
            return plugin.open(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "close")) {
            return plugin.close(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "get")) {
            return plugin.get(idam_plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int imas_uda_plugins::MappingPlugin::help(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    const char* help = "\ntemplatePlugin: Add Functions Names, Syntax, and Descriptions\n\n";
    const char* desc = "templatePlugin: help = description of this plugin";

    return setReturnDataString(plugin_interface->data_block, help, desc);
}

/**
 * Plugin version
 * @param idam_plugin_interface
 * @return
 */
int imas_uda_plugins::MappingPlugin::version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int imas_uda_plugins::MappingPlugin::build_date(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int imas_uda_plugins::MappingPlugin::default_method(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int imas_uda_plugins::MappingPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}

int imas_uda_plugins::MappingPlugin::open(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK* request_block = plugin_interface->request_block;

    int shot;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    return 0;
}

int imas_uda_plugins::MappingPlugin::close(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK* request_block = plugin_interface->request_block;

    int shot;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    return 0;
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
        auto temp = (char*)malloc(len+1);
        snprintf(temp, len+1, "%s%d;", string, indices[i]);
        free(string);
        string = temp;
    }

    string[strlen(string)-1] = '\0'; // remove last ';'
    return string;
}

int convert_IMAS_to_UDS_type(const std::string& type)
{
    if (type == "int") {
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

int imas_uda_plugins::MappingPlugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK* request_block = plugin_interface->request_block;

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

    char* path = NULL;
    int* indices = NULL;
    size_t num_indices = extract_array_indices(group, &path, &indices);
    char* indices_string = indices_to_string(indices, num_indices);

    int uda_type = convert_IMAS_to_UDS_type(type);

    auto request = boost::format("read(element=%s, shot=%d, indices=%s, dtype=%d, IDS_version='')")
                   % element % shot % indices_string % uda_type;

    return callPlugin(plugin_interface->pluginList, request.str().c_str(), plugin_interface);
}
