#include "jet_summary_plugin.h"

#include <clientserver/errorLog.h>
#include <clientserver/initStructs.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <logging/logging.h>
#include <plugins/udaPlugin.h>
#include <cmath>
#include <boost/format.hpp>
#include <mdsobjects.h>

namespace {

typedef struct data_signal
{
    float* data;
    float* times;
    int n;

} DATA_SIGNAL;

int forward_to_exp2imas(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

int establish_new_timebase(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, float** time_cache, int n);

void return_times_from_cache(DATA_BLOCK* data_block, float* time_cache, int n_times);

void prepare_time_request(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, char* time_request, const char* element);

int get_mds_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, DATA_SIGNAL* mds_data, const char* element);

float* do_linear_interpolation(DATA_SIGNAL* mds_data, float* new_times, int new_length);

void replace_element(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* old_element, const char* new_element,
                     char* request);

int call_exp2imas_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* request);

float* get_exp2imas_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* request, int* data_n, int* err);

int get_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

int data_in_xml(char* element);

int set_return_1d_float(DATA_BLOCK* data_block, const float* data, int n);

DIMS* set_compressed_dims(int n);

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
    // could cache magnetics and equilibrium time base too

    // ----------------------------------------------------------------------------------------
    // Heap Housekeeping

    if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
        if (!init) {
            // Not previously initialised: Nothing to do!
            return 0;
        }
        if (times != nullptr) { free((void*)times); }
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
    const char* help = PLUGIN_NAME ": constructs the summary ids by interpolating jet mdsplus signals\n\n";
    const char* desc = PLUGIN_NAME ": help = plugin used to construct the summary IDS for JET data";

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
        char* tmp = StringReplaceAll((const char*)in, replace, with);
        sprintf(out, "%s", tmp);
        free(tmp);
    } else {
        sprintf(out, "%s", in);
    }
}

int forward_to_exp2imas(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    char request[STRING_LENGTH];
    sprintf(request, "EXP2IMAS::%s", request_block->signal);

    return callPlugin(idam_plugin_interface->pluginList, request, idam_plugin_interface);
}

int establish_new_timebase(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, float** time_cache, int n_times)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;
    const char* element = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);

    char request[STRING_LENGTH];
    replace_element(idam_plugin_interface, element, "equilibrium/time", request);
    int err = call_exp2imas_plugin(idam_plugin_interface, request);
    if (err != 0) {
        return err;
    }

    DATA_BLOCK* data_block = idam_plugin_interface->data_block;
    int n_total = data_block->data_n;

    auto time_data = (float*)data_block->data;
    float start_time = time_data[0];
    float end_time = time_data[n_total - 1];

    auto new_times = (float*)malloc(n_times * sizeof(float));
    int i;
    float dt = (end_time - start_time) / (float)(n_times - 1);
    for (i = 0; i < n_times; i++) {
        new_times[i] = start_time + (dt * i);
    }

    free((void*)time_data);
    *time_cache = new_times;
    return 0;
}

void return_times_from_cache(DATA_BLOCK* data_block, float* time_cache, int n_times)
{
    set_return_1d_float(data_block, time_cache, n_times);
    time_cache = nullptr;
}

void prepare_time_request(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, char* time_request, const char* element)
{
    char signal_name[STRING_LENGTH];
    replace(signal_name, element, "value", "time");
    replace_element(idam_plugin_interface, element, signal_name, time_request);
}

int get_mds_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, DATA_SIGNAL* mds_data, const char* element)
{
    // get data
    int err = forward_to_exp2imas(idam_plugin_interface);
    if (err == 0) {
        DATA_BLOCK* data_block = idam_plugin_interface->data_block;
        mds_data->data = (float*)data_block->data;
        mds_data->n = data_block->data_n;

        data_block->data = nullptr;
        if (data_block->dims != nullptr) { free((void*)data_block->dims); }
        data_block->dims = nullptr;
        initDataBlock(data_block);
    } else {
        return err;
    }

    // get time data
    char time_request[STRING_LENGTH];
    prepare_time_request(idam_plugin_interface, time_request, element);
    err = call_exp2imas_plugin(idam_plugin_interface, time_request);
    if (err == 0) {
        DATA_BLOCK* data_block = idam_plugin_interface->data_block;
        mds_data->times = (float*)data_block->data;

        data_block->data = nullptr;
        if (data_block->dims != nullptr) { free((void*)data_block->dims); }
        data_block->dims = nullptr;
        initDataBlock(data_block);
    } else {
        return err;
    }

    return 0;
}

float* do_linear_interpolation(DATA_SIGNAL* mds_data, float* new_times, int new_length)
{
    float* old_data = mds_data->data;
    float* old_times = mds_data->times;
    int old_length = mds_data->n;

    int i = 0;
    int j = 0;
    int upper = 0;
    int lower = 0;
    float dy = 0.0;
    float dt = 1.0;
    auto new_data = (float*)malloc(new_length * sizeof(float));

    for (i = 0; i < new_length; i++) {
        //find interval
        j = 0;
        while (old_times[j] <= new_times[i] && j < old_length) {
            j++;
        }
        upper = j;
        lower = j - 1;
        //do interpolation
        dy = old_data[upper] - old_data[lower];
        dt = old_times[upper] - old_times[lower];
        new_data[i] = old_data[lower] + (dy / dt * (new_times[i] - old_times[lower]));
    }
    return new_data;
}

// currently doing 2 things...
void
replace_element(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* old_element, const char* new_element,
                char* request)
{
    char temp[STRING_LENGTH];
    sprintf(temp, "EXP2IMAS::%s", idam_plugin_interface->request_block->signal);
    replace(request, temp, old_element, new_element);
}

int call_exp2imas_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* request)
{
    initDataBlock(idam_plugin_interface->data_block);
    return callPlugin(idam_plugin_interface->pluginList, request, idam_plugin_interface);
}

float* get_exp2imas_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, const char* request, int* data_n, int* err)
{
    *err = call_exp2imas_plugin(idam_plugin_interface, request);
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    if (*err == 0) {
        float* data = (float*)data_block->data;
        data_block->data = nullptr;
        if (data_block->dims != nullptr) { free((void*)data_block->dims); }
        data_block->dims = nullptr;
        *data_n = data_block->data_n;
        initDataBlock(data_block);
        return data;
    } else {
        return nullptr;
    }
}

int get_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;
    int shot = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    char host[100];
    strcpy(host, "mdsplus.jet.efda.org:8000");

    MDSplus::Connection* conn = nullptr;

    try {
        conn = new MDSplus::Connection((char*)host);
    } catch (MDSplus::MdsException& ex) {
        return -1;
    }
    char signal[250];
    sprintf(signal, "_s=jet(\"PPF/EFIT/BVAC\",%d);PPFINF(_shot,_seq,_iwdat);_sig=_iwdat[1]", shot);
    fprintf(stderr, "fetching signal %s", signal);
    //int* date_time = (int*)malloc(sizeof(int));
    int date_time = -1;
    try {
        MDSplus::Data* data = conn->get(signal);
        date_time = data->getInt();
        MDSplus::deleteData(data);
    } catch (MDSplus::MdsException& ex) {
        fprintf(stderr, "-> unable to get signal :( \n");
    }

    delete conn;
    conn = nullptr;

    int len = 6;
    len++;
    char* data = (char*)malloc(len * sizeof(char));
    sprintf(data, "%d", date_time);

    DATA_BLOCK* data_block = idam_plugin_interface->data_block;
    initDataBlock(data_block);
    data_block->data = (char*)data;
    data_block->data_n = len;
    data_block->data_type = UDA_TYPE_STRING;
    data_block->rank = 1;
    if (data_block->dims != nullptr) { free((void*)data_block->dims); }
    data_block->dims = set_compressed_dims(len);

    return 0;
}

int data_in_xml(char* element)
{
    int is_ids_properties = strstr(element, "summary/ids_properties") != nullptr;
    int is_r0 = StringEndsWith(element, "summary/global_quantities/r0/value");
    int is_source = StringEndsWith(element, "source");

    return (is_ids_properties || is_r0 || is_source);
}

int set_return_1d_float(DATA_BLOCK* data_block, const float* data, int n)
{
    initDataBlock(data_block);
    data_block->data = (char*)data;
    data_block->data_n = n;
    data_block->data_type = UDA_TYPE_FLOAT;
    data_block->rank = 1;

    DIMS* dims = data_block->dims;
    if (dims != nullptr) { free((void*)dims); }
    data_block->dims = set_compressed_dims(n);
    return 0;
}

DIMS* set_compressed_dims(int n)
{
    DIMS* new_dims = (DIMS*)malloc(sizeof(DIMS));
    initDimBlock(new_dims);

    new_dims->data_type = UDA_TYPE_UNSIGNED_INT;
    new_dims->dim_n = n;
    new_dims->compressed = 1;
    new_dims->method = 0;
    new_dims->dim0 = 0.0;
    new_dims->diff = 1.0;
    new_dims->dim = (char*)nullptr;

    return new_dims;
}

int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface, float** times)
{
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* element = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);

    if (strstr(element, "ids_properties/creation_date") != nullptr) {

        return get_date(idam_plugin_interface);

    } else if (data_in_xml((char*)element)) { // nothing extra to do

        return forward_to_exp2imas(idam_plugin_interface);

    } else { // data retrieval and interpolation required

        int n_times = 100;
        int times_not_cached = (*times == nullptr);

        if (times_not_cached) {
            int err = establish_new_timebase(idam_plugin_interface, times, n_times);
            if (err != 0) {
                RAISE_PLUGIN_ERROR("Could not get data for constructing time vector");
            }
        }

        int time_data_requested = (StringEndsWith(element, "summary/time"));
        //int interpolation_required = !time_data_requested;

        if (time_data_requested) {

            // time data is final element requested by imas -- cache is cleared in this operation
            return_times_from_cache(data_block, *times, n_times);
            return 0;

        } else { // interpolation_required

            DATA_SIGNAL mds_data;
            int err = get_mds_data(idam_plugin_interface, &mds_data, (char*)element);
            if (err != 0) {
                RAISE_PLUGIN_ERROR("could not retrieve experimental data");
            }
            float* summary_data = do_linear_interpolation(&mds_data, *times, n_times);
            set_return_1d_float(data_block, summary_data, n_times);

            free((void*)mds_data.data);
            free((void*)mds_data.times);
            return 0;
        }
    }
}

} // anon namespace