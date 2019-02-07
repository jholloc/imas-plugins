#ifndef UDA_PLUGINS_IMAS_PARTIAL_PLUGIN_H
#define UDA_PLUGINS_IMAS_PARTIAL_PLUGIN_H

#include <plugins/udaPlugin.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THISPLUGIN_VERSION                  1
#define THISPLUGIN_MAX_INTERFACE_VERSION    1        // Interface versions higher than this will not be understood!
#define THISPLUGIN_DEFAULT_METHOD           "help"

int imasPartial(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

#ifdef __cplusplus
}
#endif

#endif // UDA_PLUGINS_IMAS_PARTIAL_PLUGIN_H
