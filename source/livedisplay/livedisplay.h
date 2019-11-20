#ifndef UDA_PLUGINS_LIVEDISPLAY_LIVEDISPLAY_H
#define UDA_PLUGINS_LIVEDISPLAY_LIVEDISPLAY_H

#include <plugins/pluginStructs.h>
#include <structures/genStructs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THISPLUGIN_VERSION                  1
#define THISPLUGIN_MAX_INTERFACE_VERSION    1
#define THISPLUGIN_DEFAULT_METHOD        "get"

int livedisplay(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

#ifdef __cplusplus
}
#endif

#endif // UDA_PLUGINS_LIVEDISPLAY_LIVEDISPLAY_H
