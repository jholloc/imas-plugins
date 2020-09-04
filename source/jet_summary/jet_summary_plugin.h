#ifndef UDA_PLUGIN_JET_SUMMARY_JET_SUMMARY_PLUGIN_H
#define UDA_PLUGIN_JET_SUMMARY_JET_SUMMARY_PLUGIN_H

#include <plugins/pluginStructs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THISPLUGIN_MAX_INTERFACE_VERSION    1        // Interface versions higher than this will not be understood!
#define THISPLUGIN_DEFAULT_METHOD           "help"

int jetSummaryPlugin(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

#ifdef __cplusplus
}
#endif

#endif // UDA_PLUGIN_JET_EQUILIBRIUM_JET_EQUILIBRIUM_PLUGIN_H
