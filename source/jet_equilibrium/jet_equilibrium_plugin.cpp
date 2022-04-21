#include "jet_equilibrium_plugin.h"

#include <clientserver/errorLog.h>
#include <clientserver/initStructs.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <logging/logging.h>
#include <plugins/udaPlugin.h>
#include <cmath>
#include <boost/format.hpp>

namespace {


int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);


} // anon namespace

#ifndef strndup

char*
strndup(const char* s, size_t n)
{
    char* result;
    size_t len = strlen(s);

    if (n < len) {
        len = n;
    }

    result = (char*)malloc(len + 1);
    if (!result) {
        return nullptr;
    }

    result[len] = '\0';
    return (char*)memcpy(result, s, len);
}

#endif

int jetEquilibriumPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    // ----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    idam_plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, nullptr, 10);

    REQUEST_DATA* request_data = idam_plugin_interface->request_data;

    static short init = 0;

    // ----------------------------------------------------------------------------------------
    // Heap Housekeeping

    if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_data->function, "reset")) {
        if (!init) {
            // Not previously initialised: Nothing to do!
            return 0;
        }

        // Free Heap & reset counters
        init = 0;
        return 0;
    }

    // ----------------------------------------------------------------------------------------
    // Initialise

    if (!init || STR_IEQUALS(request_data->function, "init")
        || STR_IEQUALS(request_data->function, "initialise")) {

        init = 1;
        if (STR_IEQUALS(request_data->function, "init")
            || STR_IEQUALS(request_data->function, "initialise")) {
            return 0;
        }
    }

    // ----------------------------------------------------------------------------------------
    // Plugin Functions
    // ----------------------------------------------------------------------------------------

    int err = 0;

    if (STR_IEQUALS(request_data->function, "help")) {
        err = do_help(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "version")) {
        err = do_version(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "builddate")) {
        err = do_builddate(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "defaultmethod")) {
        err = do_defaultmethod(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "maxinterfaceversion")) {
        err = do_maxinterfaceversion(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "read")) {
        err = do_read(idam_plugin_interface);
    } else {
        RAISE_PLUGIN_ERROR("Unknown function requested!");
    }

    return err;
}

namespace {

// Help: A Description of library functionality
int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    // const char* help = PLUGIN_NAME ": this plugin maps Tore Supra data to IDS\n\n";
    const char* help = PLUGIN_NAME ": this text has been modified\n\n";
    const char* desc = PLUGIN_NAME ": help = plugin used for mapping Tore Supra experimental data to IDS";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin version number";

    return setReturnDataString(idam_plugin_interface->data_block, PLUGIN_VERSION, desc);
}

// Plugin Build Date
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin build date";

    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, desc);
}

// Plugin Default Method
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin default method";

    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, desc);
}

// Plugin Maximum Interface Version
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Maximum Interface Version";

    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, desc);
}

void replace(char* out, const char* in, const char* replace, const char* with)
{
    const char* pos = strstr(in, replace);
    if (pos != nullptr) {
        char* tmp = StringReplaceAll(in, replace, with);
        sprintf(out, "%s", tmp);
        free(tmp);
    } else {
        sprintf(out, "%s", in);
    }
}

int call_exp2imas_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* signal_name){
    
    char temp[STRING_LENGTH];
    sprintf(temp, "EXP2IMAS::%s", idam_plugin_interface->request_data->signal);

    char request[STRING_LENGTH];
    replace(request, temp, "rho_volume_norm", signal_name );

    initDataBlock(idam_plugin_interface->data_block);
    return callPlugin(idam_plugin_interface->pluginList, request, idam_plugin_interface);
}

float* get_exp2imas_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* signal_name, int* err){
    
    *err = call_exp2imas_plugin(idam_plugin_interface, signal_name);
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    if (*err == 0){
        float* data = (float*)data_block->data;
        data_block->data = nullptr;
        return data;
    } else {
        return nullptr;
    }
}

float get_exp2imas_scalar(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* signal_name, int* err){
    
    *err = call_exp2imas_plugin(idam_plugin_interface, signal_name);
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    if (*err == 0){
        float data = ((float*)data_block->data)[0];
        free((void*)data_block->data);
        data_block->data = nullptr;
        return data;
    } else {
        return 0.0;
    }
}

float* integrate_gradients(float* gradients, float dx, int n){

    float* values = (float*)malloc(n * sizeof(float));
    values[0] = 0.0;
    
    int i;
    for (i = 1; i < n - 1; i++){
        values[i] = values[i-1] + 0.5*(gradients[i-1] + gradients[i]) * dx;
    }

    return values;
}

// ----------------------------------------------------------------------------------------
// Add functionality here ....
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;
    REQUEST_DATA* request_data = idam_plugin_interface->request_data;

    const char* element = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_data->nameValueList, element);

    if (StringEndsWith(element, "/profiles_1d/rho_volume_norm")) {
        int err;
        float lcfs_vol = get_exp2imas_scalar(idam_plugin_interface, "lcfs_vol", &err);
        if (err != 0) return err;

        float* vjac = get_exp2imas_data(idam_plugin_interface, "vjac", &err);
        if (err != 0) return err;

        int n = data_block->data_n;
        float dpsi_n = 1.0 / (n-1);
        
        float* volumes = integrate_gradients(vjac, dpsi_n, n);
        volumes[n-1] = lcfs_vol;

        int i;
        float* data = (float*)malloc(n * sizeof(float));
        for (i = 0; i < n; i++){
            data[i] = std::sqrt(volumes[i] / lcfs_vol );
        }

        free((void*)vjac);
        free((void*)volumes);
        data_block->data = (char*)data;
    } else {
        auto request = boost::format("EXP2IMAS::%s") % request_data->signal;
        return callPlugin(idam_plugin_interface->pluginList, request.str().c_str(), idam_plugin_interface);
    }

    return 0;
}

} // anon namespace
