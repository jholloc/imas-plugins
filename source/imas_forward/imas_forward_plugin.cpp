#include "imas_forward_plugin.h"

#include <memory>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <boost/range/adaptor/reversed.hpp>
#include <libgen.h>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <UDA.hpp>

namespace {

int forward_request(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* host)
{
    std::stringstream ss;
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    ss << request_block->format << "::" << request_block->function << "(";
    std::string delim;
    for (int i = 0; i < request_block->nameValueList.listSize; ++i) {
        ss << delim << request_block->nameValueList.nameValue[i].pair;
        delim = ",";
    }
    ss << ")";

    uda::Client::setServerHostName(host);
    uda::Client client;
    try {
        const uda::Result& res = client.get(ss.str(), "");
        return res.errorCode();
    } catch (uda::UDAException& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

class IMASForwardPlugin {
public:
    int init(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
    {
        const char* host = nullptr;
        FIND_REQUIRED_STRING_VALUE(idam_plugin_interface->request_block->nameValueList, host);
        host_ = host;
        return 0;
    }

    void reset()
    {
        host_ = nullptr;
    }

    const char* host()
    {
        return host_;
    }

    int help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

private:
    const char* host_ = nullptr;
};

}

int imasForwardPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static IMASForwardPlugin plugin{};

        if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
            RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        }

        idam_plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, nullptr, 10);

        REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

        std::string function = request_block->function;

        if (idam_plugin_interface->housekeeping || function == "reset") {
            plugin.reset();
            return 0;
        }

        if (function == "init" || function == "initialise") {
            return plugin.init(idam_plugin_interface);
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
        } else if (function == "openPulse"
                   || function == "closePulse"
                   || function == "beginAction"
                   || function == "endAction"
                   || function == "writeData"
                   || function == "readData"
                   || function == "deleteData"
                   || function == "beginArraystructAction") {
            return forward_request(idam_plugin_interface, plugin.host());
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
int IMASForwardPlugin::help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    std::string path = __FILE__;
    std::string dir = dirname((char*)path.c_str());
    path = dir + "/help.md";
    std::ifstream ifs(path);
    std::stringstream ss;
    ss << ifs.rdbuf();

    const char* help = ss.str().c_str();
    const char* desc = PLUGIN_NAME ": help = description of this plugin";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

/**
 * Plugin version
 * @param idam_plugin_interface
 * @return
 */
int IMASForwardPlugin::version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, PLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int IMASForwardPlugin::build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int IMASForwardPlugin::default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int IMASForwardPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

}
