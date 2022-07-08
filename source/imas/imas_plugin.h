#ifndef UDA_PLUGINS_IMAS_FORWARD_PLUGIN_H
#define UDA_PLUGINS_IMAS_FORWARD_PLUGIN_H

#include <plugins/udaPlugin.h>

#define THISPLUGIN_MAX_INTERFACE_VERSION    1        // Interface versions higher than this will not be understood!
#define THISPLUGIN_DEFAULT_METHOD           "help"

#define PATH_LEN 2048

#ifdef __cplusplus
extern "C" {
#endif

int imasPlugin(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

struct Data {
    char name[PATH_LEN];
    const unsigned char* data = nullptr;
    int rank = 0;
    int dims[64] = {0};
    int datatype = 0;
};

struct DataList {
    Data* list;
    int size;
};

#ifdef __cplusplus
}

#endif

#endif // UDA_PLUGINS_IMAS_FORWARD_PLUGIN_H
