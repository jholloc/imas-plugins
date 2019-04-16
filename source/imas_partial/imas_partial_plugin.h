#ifndef UDA_PLUGINS_IMAS_PARTIAL_PLUGIN_H
#define UDA_PLUGINS_IMAS_PARTIAL_PLUGIN_H

#include <plugins/udaPlugin.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THISPLUGIN_MAX_INTERFACE_VERSION    1        // Interface versions higher than this will not be understood!
#define THISPLUGIN_DEFAULT_METHOD           "help"

int imasPartial(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

#ifdef __cplusplus
}
#endif

struct Data {
    const char* name = nullptr;
    const unsigned char* data = nullptr;
    int rank = 0;
    int dims[64] = {0};
    int datatype = 0;
};

struct DataList {
    Data* list;
    int size;
};

#endif // UDA_PLUGINS_IMAS_PARTIAL_PLUGIN_H
