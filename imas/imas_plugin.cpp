#include "imas_plugin.h"

#include <cstdlib>
#include <cstring>
#include <boost/range/adaptor/reversed.hpp>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>

#include <ual_backend.h>
#include <ual_lowlevel.h>

static int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

static int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

static int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

static int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

static int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

static int do_openPulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_closePulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_beginAction(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_endAction(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_writeData(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_readData(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_deleteData(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_beginArraystructAction(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

int imasPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    static int init = 0;

    //----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

    //----------------------------------------------------------------------------------------
    // Heap Housekeeping

    // Plugin must maintain a list of open file handles and sockets: loop over and close all files and sockets
    // Plugin must maintain a list of plugin functions called: loop over and reset state and free heap.
    // Plugin must maintain a list of calls to other plugins: loop over and call each plugin with the housekeeping request
    // Plugin must destroy lists at end of housekeeping

    // A plugin only has a single instance on a server. For multiple instances, multiple servers are needed.
    // Plugins can maintain state so recursive calls (on the same server) must respect this.
    // If the housekeeping action is requested, this must be also applied to all plugins called.
    // A list must be maintained to register these plugin calls to manage housekeeping.
    // Calls to plugins must also respect access policy and user authentication policy

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
        if (!init) return 0; // Not previously initialised: Nothing to do!
        // Free Heap & reset counters
        init = 0;
        return 0;
    }

    //----------------------------------------------------------------------------------------
    // Initialise

    if (!init || STR_IEQUALS(request_block->function, "init")
        || STR_IEQUALS(request_block->function, "initialise")) {

        init = 1;
        if (STR_IEQUALS(request_block->function, "init") || STR_IEQUALS(request_block->function, "initialise"))
            return 0;
    }

    //----------------------------------------------------------------------------------------
    // Plugin Functions
    //----------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------
    // Standard methods: version, builddate, defaultmethod, maxinterfaceversion

    if (STR_IEQUALS(request_block->function, "help")) {
        return do_help(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "version")) {
        return do_version(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "builddate")) {
        return do_builddate(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "defaultmethod")) {
        return do_defaultmethod(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "maxinterfaceversion")) {
        return do_maxinterfaceversion(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "openPulse")) {
        return do_openPulse(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "closePulse")) {
        return do_closePulse(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "beginAction")) {
        return do_beginAction(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "endAction")) {
        return do_endAction(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "writeData")) {
        return do_writeData(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "readData")) {
        return do_readData(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "deleteData")) {
        return do_deleteData(idam_plugin_interface);
    } else if (STR_IEQUALS(request_block->function, "beginArraystructAction")) {
        return do_beginArraystructAction(idam_plugin_interface);
    } else {
        RAISE_PLUGIN_ERROR("Unknown function requested!");
    }
}

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* help = "\ntemplatePlugin: Add Functions Names, Syntax, and Descriptions\n\n";
    const char* desc = "templatePlugin: help = description of this plugin";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

/**
 * Plugin version
 * @param idam_plugin_interface
 * @return
 */
int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}

int do_openPulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int backend_id;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, backend_id);

    int shot;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, user);

    const char* tokamak;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, tokamak);

    const char* version;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, version);

    int mode;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, mode);

    const char* options;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, options);

    int ctxId = Lowlevel::beginPulseAction(backend_id, shot, run, user, tokamak, version);
    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->openPulse(dynamic_cast<PulseContext*>(env.context), mode, options);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int do_closePulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    int mode;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, mode);

    const char* options;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, options);

    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->closePulse(dynamic_cast<PulseContext*>(env.context), mode, options);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int do_beginAction(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    const char* dataObject;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, dataObject);

    int access;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, access);

    int range;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, range);

    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    LLenv env = Lowlevel::getLLenv(ctxId);

    OperationContext opCtx(*dynamic_cast<PulseContext*>(env.context), dataObject, access, range, time, interp);

    env.backend->beginAction(&opCtx);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int do_endAction(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->endAction(env.context);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int do_writeData(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    const char* field;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, field);

    const char* timebase;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, timebase);

    void* data = nullptr;

    int datatype;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, datatype);

    int* dims;
    size_t ndims;
    FIND_REQUIRED_INT_ARRAY(request_block->nameValueList, dims);

    int rank = static_cast<int>(ndims);

    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->writeData(env.context, field, timebase, data, datatype, rank, dims);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int do_readData(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    LLenv env = Lowlevel::getLLenv(ctxId);

    const char* field;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, field);

    const char* timebase;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, timebase);

    void* data = nullptr;

    int datatype;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, datatype);

    int rank;
    int dims[64];

    env.backend->readData(env.context, field, timebase, &data, &datatype, &rank, dims);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int do_deleteData(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    const char* path;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    const char* dataObject;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, dataObject);

    int access;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, access);

    int range;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, range);

    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    LLenv env = Lowlevel::getLLenv(ctxId);

    OperationContext opCtx(*dynamic_cast<PulseContext*>(env.context), dataObject, access, range, time, interp);

    env.backend->deleteData(&opCtx, path);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

static bool is_integer(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    char* end = nullptr;
    std::strtol(string.c_str(), &end, 10);
    return end != nullptr && *end == '\0';
}

ArraystructContext* build_arraystruct_context(const char* path, const char* timebase, const PulseContext& ctx, const std::string& dataobject, int access)
{
    std::vector<std::string> tokens;

    std::stringstream ss(path);
    std::string fragment;
    while (std::getline(ss, fragment, '/')) {
        tokens.push_back(fragment);
    }

    OperationContext opCtx(ctx, dataobject, access);
    ArraystructContext* arrCtx = nullptr;
    std::string prev;

    for (const auto& token : tokens) {
        if (is_integer(token)) {
            if (prev.empty()) {
                THROW_ERROR(nullptr, "Invalid path found in arraystruct context");
            }
            auto index = static_cast<int>(std::strtol(token.c_str(), nullptr, 10));
            auto temp = new ArraystructContext(opCtx, prev, timebase, arrCtx->getParent(), index);
            delete arrCtx;
            arrCtx = temp;
        } else {
            arrCtx = new ArraystructContext(opCtx, token, timebase, arrCtx);
        }

        prev = token;
    }

    return arrCtx;
}

int do_beginArraystructAction(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    int size;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, size);

    const char* dataObject;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, dataObject);

    int access;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, access);

    int range;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, range);

    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    const char* path;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    const char* timebase;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, timebase);

    LLenv env = Lowlevel::getLLenv(ctxId);

    auto pulseCtx = dynamic_cast<PulseContext*>(env.context);
    OperationContext opCtx(*dynamic_cast<PulseContext*>(env.context), dataObject, access, range, time, interp);

    ArraystructContext* arrayCtx = build_arraystruct_context(path, timebase, *pulseCtx, dataObject, access);

    env.backend->beginArraystructAction(arrayCtx, &size);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}
