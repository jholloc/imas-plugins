#include "test_helpers.h"

#include <clientserver/makeRequestBlock.h>
#include <plugins/udaPlugin.h>
#include <uda.h>
#include <structures/struct.h>

#ifndef UDA_PLUGIN_OPERATIONAL
#define UDA_PLUGIN_CLASS_FUNCTION   PLUGINFUNCTION
#define UDA_PLUGIN_OPERATIONAL      PLUGINOPERATIONAL
#endif

void uda::test::add_plugin(PLUGINLIST* plugin_list, const std::string& request, PLUGINFUNP func)
{
    plugin_list->plugin = (PLUGIN_DATA*)realloc(plugin_list->plugin, (plugin_list->mcount + 1) * sizeof(PLUGIN_DATA));
    memset(&plugin_list->plugin[plugin_list->mcount], '\0', sizeof(PLUGIN_DATA));
    ++plugin_list->mcount;
    ++plugin_list->count;

    size_t r = request.find("::");
    if (r != std::string::npos) {
        std::string s = request.substr(0, r);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        sprintf(plugin_list->plugin->format, "%s", s.c_str());
    }

    plugin_list->plugin->request = 1;
    plugin_list->plugin->plugin_class = UDA_PLUGIN_CLASS_FUNCTION;
    plugin_list->plugin->status = UDA_PLUGIN_OPERATIONAL;
    plugin_list->plugin->idamPlugin = func;
}

IDAM_PLUGIN_INTERFACE uda::test::generate_plugin_interface(const char* request)
{
    IDAM_PLUGIN_INTERFACE plugin_interface = {};

    auto data_block = (DATA_BLOCK*)calloc(1, sizeof(DATA_BLOCK));
    auto client_block = (CLIENT_BLOCK*)calloc(1, sizeof(CLIENT_BLOCK));
    auto request_block = (REQUEST_BLOCK*)calloc(1, sizeof(REQUEST_BLOCK));
    auto data_source = (DATA_SOURCE*)calloc(1, sizeof(DATA_SOURCE));
    auto signal_desc = (SIGNAL_DESC*)calloc(1, sizeof(SIGNAL_DESC));
    auto environment = (ENVIRONMENT*)calloc(1, sizeof(ENVIRONMENT));
    auto plugin_list = (PLUGINLIST*)calloc(1, sizeof(PLUGINLIST));
    auto userdefinedtypelist = (USERDEFINEDTYPELIST*)calloc(1, sizeof(USERDEFINEDTYPELIST));
    auto logmalloclist = (LOGMALLOCLIST*)calloc(1, sizeof(LOGMALLOCLIST));

    initIdamDataBlock(data_block);
    initClientBlock(client_block, 7, "test");
    initRequestBlock(request_block);
    initDataSource(data_source);
    initSignalDesc(signal_desc);
    initUserDefinedTypeList(userdefinedtypelist);
    initLogMallocList(logmalloclist);

    add_plugin(plugin_list, request, nullptr);

    sprintf(environment->api_device, "%s", "MAST");
    sprintf(environment->api_delim, "%s", "::");

    sprintf(request_block->signal, "%s", request);
    make_request_block(request_block, *plugin_list, environment);

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
