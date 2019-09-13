#include "test_helpers.h"

#include <cstdarg>
#include <clientserver/makeRequestBlock.h>
#include <plugins/udaPlugin.h>

#ifndef UDA_PLUGIN_OPERATIONAL
#define UDA_PLUGIN_CLASS_FUNCTION   PLUGINFUNCTION
#define UDA_PLUGIN_OPERATIONAL      PLUGINOPERATIONAL
#endif

IDAM_PLUGIN_INTERFACE uda::test::generate_plugin_interface(const char* request)
{
    IDAM_PLUGIN_INTERFACE plugin_interface = {};

    auto data_block = new DATA_BLOCK;
    auto client_block = new CLIENT_BLOCK;
    auto request_block = new REQUEST_BLOCK;
    auto data_source = new DATA_SOURCE;
    auto signal_desc = new SIGNAL_DESC;
    auto environment = new ENVIRONMENT;
    auto plugin_list = new PLUGINLIST;
    auto userdefinedtypelist = new USERDEFINEDTYPELIST;
    auto logmalloclist = new LOGMALLOCLIST;

    plugin_list->count = 1;
    plugin_list->plugin = new PLUGIN_DATA;

    std::string s_request = request;
    size_t r = s_request.find("::");
    if (r != std::string::npos) {
        std::string s = s_request.substr(0, r);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        sprintf(plugin_list->plugin->format, "%s", s.c_str());
    }

    plugin_list->plugin->request = 1;
    plugin_list->plugin->plugin_class = UDA_PLUGIN_CLASS_FUNCTION;
    plugin_list->plugin->status = UDA_PLUGIN_OPERATIONAL;

    sprintf(environment->api_device, "%s", "MAST");
    sprintf(environment->api_delim, "%s", "::");

    sprintf(request_block->signal, "%s", request);
    makeRequestBlock(request_block, *plugin_list, environment);

    plugin_interface.interfaceVersion = 1;
    plugin_interface.pluginVersion = 0;
    plugin_interface.data_block = data_block;
    plugin_interface.client_block = client_block;
    plugin_interface.request_block = request_block;
    plugin_interface.data_source = data_source;
    plugin_interface.signal_desc = signal_desc;
    plugin_interface.environment = environment;
    plugin_interface.housekeeping = 0;
    plugin_interface.changePlugin = 0;
    plugin_interface.pluginList = plugin_list;
    plugin_interface.userdefinedtypelist = userdefinedtypelist;
    plugin_interface.logmalloclist = logmalloclist;

    return plugin_interface;
}
