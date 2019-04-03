#if 0
g++ -O0 -DFATCLIENT -g $(pkg-config --cflags uda-cpp) $(pkg-config --cflags imas-cpp) $0 -o test.x $(pkg-config --libs uda-cpp) $(pkg-config --libs imas-cpp) -L$UDA_PLUGIN_DIR -limas_partial_plugin
exit $?
#endif

#include <string>
#include <vector>
#include <deque>
#include <cassert>
#include <boost/algorithm/string.hpp>

#include <clientserver/initStructs.h>
#include <server/makeServerRequestBlock.h>

#include <ual_lowlevel.h>
#include <ual_backend.h>

#include "imas_partial_plugin.h"

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
    plugin_list.plugin[0].plugin_class = PLUGINFUNCTION;
    strcpy(plugin_list.plugin[0].format, "IMAS_PARTIAL");

    makeServerRequestBlock(interface.request_block, plugin_list);

    return interface;
}

int main()
{
    IDAM_PLUGIN_INTERFACE plugin_interface = generate_plugin_interface("imas_partial::open()");
    int rc = imasPartial(&plugin_interface);
    assert(rc == 0);

    plugin_interface = generate_plugin_interface("imas_partial::get(path=/magnetics/flux_loop/3/flux/data)");

    rc = imasPartial(&plugin_interface);
    assert(rc == 0);
   
    return 0;
}
