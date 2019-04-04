#include "imasUdaPlugin.h"

#include <string>
#include <boost/format.hpp>
#include <iostream>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>

class ImasUdaPlugin
{
public:
    void init();
    void reset();

    int help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int get(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

private:
    bool initialised_ = false;
};

int imasUdaPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    static ImasUdaPlugin plugin;

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
    } else if (STR_IEQUALS(request_block->function, "read")) {
        return plugin.read(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "get")) {
        return plugin.get(idam_plugin_interface);
    } else {
        RAISE_PLUGIN_ERROR("Unknown function requested!");
    }
}

/**
 * Initialise the plugin
 */
void ImasUdaPlugin::init()
{
    if (!initialised_) {
        // Do initialisation
    }
    initialised_ = true;
}

/**
 * Reset the plugin
 */
void ImasUdaPlugin::reset()
{
    if (initialised_) {
        // Do reset
    }
    initialised_ = false;
}

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int ImasUdaPlugin::help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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
int ImasUdaPlugin::version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int ImasUdaPlugin::build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int ImasUdaPlugin::default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int ImasUdaPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}

namespace {

char* insertNodeIndices(const char* expr, int** indices, size_t* n_indices)
{
    char* indexedExpr = strdup(expr);

    if ((*indices) == nullptr) {
        return indexedExpr;
    }

    const char* p;
    size_t n = 0;

    while ((p = strstr(indexedExpr, "#")) != nullptr) {
        auto len = (size_t)snprintf(nullptr, 0, "%d", (*indices)[n]);
        char num_str[len + 1];
        snprintf(num_str, len + 1, "%d", (*indices)[n]);
        ++n;

        char* pre = strndup(indexedExpr, (p - indexedExpr));

        len = strlen(pre) + strlen(num_str) + strlen(p + 1) + 1;
        char* temp = (char*)malloc((len + 1) * sizeof(char));
        snprintf(temp, len, "%s%s%s", pre, num_str, p + 1);
        free(indexedExpr);
        indexedExpr = temp;

        free(pre);
    }

    if (n == *n_indices) {
        free(*indices);
        *indices = nullptr;
        *n_indices = 0;
    } else {
        int* temp = (int*)malloc((*n_indices - n) * sizeof(int));
        size_t i;
        for (i = n; i < *n_indices; ++i) {
            temp[i - n] = (*indices)[i];
        }
        free(*indices);
        *indices = temp;
        *n_indices = (*n_indices - n);
    }

    return indexedExpr;
}

} // anon namespace

int ImasUdaPlugin::read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

    std::string path = insertNodeIndices(element, &indices, &nindices);
    auto request = boost::format("UDA::get(signal=%s, source=%d)") % path % shot;

    return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
}

int ImasUdaPlugin::get(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    initDataBlock(data_block);

    data_block->rank = 0;
    data_block->dims = nullptr;

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* group = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, group);

    const char* variable = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, variable);

    int shot = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    auto request = boost::format("UDA::get(signal=%s/%s, source=%d)") % group % variable % shot;

    std::cerr << request.str() << std::endl;

    return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
}
