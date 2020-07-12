#include "jet_summary_plugin.h"

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
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, float** times);


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

int jetSummaryPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    // ----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    idam_plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, nullptr, 10);

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    static short init = 0;
    static float* times = nullptr;

    // ----------------------------------------------------------------------------------------
    // Heap Housekeeping

    if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
        if (!init) {
            // Not previously initialised: Nothing to do!
            return 0;
        }
        if (times != nullptr)free((void*)times);
        times = nullptr;

        // Free Heap & reset counters
        init = 0;
        return 0;
    }

    // ----------------------------------------------------------------------------------------
    // Initialise

    if (!init || STR_IEQUALS(request_block->function, "init")
        || STR_IEQUALS(request_block->function, "initialise")) {

        times = nullptr;
        init = 1;
        if (STR_IEQUALS(request_block->function, "init")
            || STR_IEQUALS(request_block->function, "initialise")) {
            return 0;
        }
    }

    // ----------------------------------------------------------------------------------------
    // Plugin Functions
    // ----------------------------------------------------------------------------------------

    int err = 0;

    if (STR_IEQUALS(request_block->function, "help")) {
        err = do_help(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "version")) {
        err = do_version(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "builddate")) {
        err = do_builddate(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "defaultmethod")) {
        err = do_defaultmethod(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "maxinterfaceversion")) {
        err = do_maxinterfaceversion(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "read")) {
        err = do_read(idam_plugin_interface, &times);
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

void replace(char* out, char* in, const char* replace, const char* with)
{
    char* pos = strstr(in, replace);
    if (pos != nullptr) {
        char* tmp = StringReplaceAll((const char*) in, replace, with);
        sprintf(out, "%s", tmp);
        free(tmp);
    } else {
        sprintf(out, "%s", in);
    }
}

// currently doing 2 things...
void replace_element(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, char* old_element, char* new_element, char* request){
    char temp[STRING_LENGTH];
    sprintf(temp, "EXP2IMAS::%s", idam_plugin_interface->request_block->signal);

    replace(request, temp, (const char*) old_element, (const char*) new_element);
}

int call_exp2imas_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, char* request){

    initDataBlock(idam_plugin_interface->data_block);
    return callPlugin(idam_plugin_interface->pluginList, request, idam_plugin_interface);
}

float* get_exp2imas_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, char* request, int* data_n, int* err){
    
    *err = call_exp2imas_plugin(idam_plugin_interface, request);
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    if (*err == 0){
        float* data = (float*)data_block->data;
        data_block->data = nullptr;
        if(data_block->dims != nullptr) free((void*) data_block->dims);
        data_block->dims = nullptr;
        *data_n = data_block->data_n;
        initDataBlock(data_block);
        return data;
    } else {
        return nullptr;
    }
}


// ----------------------------------------------------------------------------------------
// Add functionality here ....
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, float** times)
{
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* element = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);
    FILE* info = fopen("/afs/eufus.eu/user/g/g2sdixon/uda/uda-2.2.5/build_0/etc/jet_summary_info.txt" , "w+");
    fprintf(info, "Input element string is:%s\n", element);
    fclose(info);

    if(strstr(element, "summary/ids_properties") != nullptr || StringEndsWith(element, "summary/global_quantities/r0/value")) {

        char request[STRING_LENGTH];
        sprintf(request, "EXP2IMAS::%s", request_block->signal);
        return callPlugin(idam_plugin_interface->pluginList, request, idam_plugin_interface);

    } else {

        // 0. establish time base and cache result
            // 0.1 read equilibrium time
            // 0.2 take first and last values
            // 0.3 use 100 points between these two values
        
        int n_times = 100;

        float* new_times = nullptr;
        if(*times == nullptr){

            char* signal_name = "equilibrium/time";
            char request[STRING_LENGTH];
            replace_element(idam_plugin_interface, (char*) element, signal_name, request);
            int err = call_exp2imas_plugin(idam_plugin_interface, request);
            if ( err != 0){
                RAISE_PLUGIN_ERROR("Could not get data for constructing time vector");
            }
            
            int n_total = data_block->data_n;

            float* time_data = (float*)data_block->data;
            float start_time = time_data[0];
            float end_time = time_data[n_total - 1];

            new_times = (float*)malloc(n_times * sizeof(float));
            int i;
            float dt = (end_time - start_time) / (float)(n_times -1);
            for(i = 0; i < n_times; i++){
                new_times[i] = start_time + (dt * i);
            }

            free((void*) time_data);
            *times = new_times;
        }

        // 1. get data (and time) using exp2imas plugin -- or return time vector from cache
        if (StringEndsWith(element, "summary/time")) {
            float* summary_data = *times;

            initDataBlock(data_block);
            data_block->data = (char*)summary_data;
            data_block->data_n = n_times;
            data_block->data_type = UDA_TYPE_FLOAT;
            data_block->rank = 1;

            DIMS* dims = data_block->dims;
            if(dims != nullptr) free((void*)dims);

            DIMS* new_dims = (DIMS *)malloc(sizeof(DIMS));
            initDimBlock(new_dims);
            new_dims->data_type = UDA_TYPE_UNSIGNED_INT;
            new_dims->dim_n = data_block->data_n;
            new_dims->compressed = 1;
            new_dims->method = 0;
            new_dims->dim0 = 0.0;
            new_dims->diff = 1.0;
            new_dims->dim = (char*)nullptr;

            data_block->dims = new_dims;
            *times = nullptr;
            return 0;
        }
        else{

            float* time_cache = *times; 
            float* all_data = nullptr;
            float* all_times = nullptr;

            char request[STRING_LENGTH];
            sprintf(request, "EXP2IMAS::%s",idam_plugin_interface->request_block->signal);

            int err;
            int n_points = -1;
            all_data = get_exp2imas_data(idam_plugin_interface, request, &n_points, &err);

            if (err != 0){
                RAISE_PLUGIN_ERROR("could not retrieve experimental data");
            }

            char time_request[STRING_LENGTH];
            char signal_name[STRING_LENGTH];
            replace(signal_name,(char*)element, "value", "time");

            replace_element(idam_plugin_interface, (char*)element, signal_name, time_request);

            int n_time_points;

            all_times = get_exp2imas_data(idam_plugin_interface, time_request, &n_time_points, &err);

            if (err != 0){
                RAISE_PLUGIN_ERROR("could not retrieve original times for interpolating experimental data");
            }

            // 2. interpolate data signal onto time base

            int i = 0;
            int j = 0;
            int upper = 0;
            int lower =0;
            float dy = 0.0;
            float dt = 1.0;
            float* summary_data = (float*)malloc(n_times * sizeof(float));

            for(i = 0; i < n_times; i++){
                // 2.1 find interval
                j = 0;
                while(all_times[j] <= time_cache[i] && j < n_points){
                    j++;
                }
                upper = j;
                lower = j - 1;
                // 2.2 interpolate (linear)
                dy = all_data[upper] - all_data[lower];
                dt = all_times[upper] - all_times[lower];
                summary_data[i] = all_data[lower] + ( dy/dt * (time_cache[i] - all_times[lower]) );
            }

            free((void*)all_data);
            free((void*)all_times);

            initDataBlock(data_block);
            data_block->data = (char*)summary_data;
            data_block->data_n = n_times;
            data_block->data_type = UDA_TYPE_FLOAT;
            data_block->rank = 1;

            DIMS* dims = data_block->dims;
            if(dims != nullptr) free((void*)dims);

            DIMS* new_dims = (DIMS *)malloc(sizeof(DIMS));
            initDimBlock(new_dims);
            new_dims->data_type = UDA_TYPE_UNSIGNED_INT;
            new_dims->dim_n = data_block->data_n;
            new_dims->compressed = 1;
            new_dims->method = 0;
            new_dims->dim0 = 0.0;
            new_dims->diff = 1.0;
            new_dims->dim = (char*)nullptr;

            data_block->dims = new_dims;

            return 0;
        }
    }
}