#ifndef UDA_PLUGINS_IMAS_PLUGIN_H
#define UDA_PLUGINS_IMAS_PLUGIN_H

#include <plugins/udaPlugin.h>

#define THISPLUGIN_MAX_INTERFACE_VERSION    1        // Interface versions higher than this will not be understood!
#define THISPLUGIN_DEFAULT_METHOD           "help"

#ifdef __cplusplus
extern "C" {
#endif

int imasPlugin(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

#ifdef __cplusplus
}
#endif

#endif // UDA_PLUGINS_IMAS_PLUGIN_H
