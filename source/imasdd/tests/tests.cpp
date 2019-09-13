#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <imasdd_plugin.h>
#include <clientserver/initStructs.h>
#include <server/makeServerRequestBlock.h>

#ifdef UDA_LEGACY
#define UDA_PLUGIN_CLASS_FUNCTION   PLUGINFUNCTION
#define UDA_PLUGIN_OPERATIONAL      PLUGINOPERATIONAL
#endif

IDAM_PLUGIN_INTERFACE generate_plugin_interface(const std::string& object)
{
    IDAM_PLUGIN_INTERFACE interface = {};

    interface.interfaceVersion = 1;

    interface.data_block = (DATA_BLOCK*)malloc(sizeof(DATA_BLOCK)); 
    initDataBlock(interface.data_block);

    interface.request_block = (REQUEST_BLOCK*)malloc(sizeof(REQUEST_BLOCK));
    initRequestBlock(interface.request_block);
    strcpy(interface.request_block->signal, object.c_str());

    PLUGINLIST plugin_list = {};

    plugin_list.count = 1;
    plugin_list.mcount = 1;
    plugin_list.plugin = (PLUGIN_DATA*)malloc(sizeof(PLUGIN_DATA));

    plugin_list.plugin[0].request = 1000;
    plugin_list.plugin[0].plugin_class = UDA_PLUGIN_CLASS_FUNCTION;
    strcpy(plugin_list.plugin[0].format, "IMAS");

    makeServerRequestBlock(interface.request_block, plugin_list);

    return interface;
}

TEST_CASE( "get root magnetics", "[MAG]" )
{
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("imas::get(path=/magnetics)");

    int rc = imasdd_plugin(&interface);

    REQUIRE( rc == 0 );
}
