#include "imas_plugin.h"

#include <memory>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <boost/range/adaptor/reversed.hpp>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>

#include <ual_backend.h>
#include <ual_lowlevel.h>

namespace {

class IMASPlugin {
public:
    void init() {}
    void reset() {}

    int help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int open_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int close_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int begin_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int end_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int write_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int read_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int delete_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int begin_arraystruct_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

private:
    std::unique_ptr<OperationContext> op_ctx_;
    std::stack<std::unique_ptr<ArraystructContext>> array_ctx_stack_;

    ArraystructContext* build_arraystruct_context(const char* path, const char* timebase);
};

}

int imasPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static IMASPlugin plugin{};

        if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
            RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        }

        idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

        REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

        std::string function = request_block->function;

        if (idam_plugin_interface->housekeeping || function == "reset") {
            plugin.reset();
            return 0;
        }

        plugin.init();

        if (function == "init" || function == "initialise") {
            return 0;
        }

        if (function == "help") {
            return plugin.help(idam_plugin_interface);
        } else if (function == "version") {
            return plugin.version(idam_plugin_interface);
        } else if (function == "builddate") {
            return plugin.build_date(idam_plugin_interface);
        } else if (function == "defaultmethod") {
            return plugin.default_method(idam_plugin_interface);
        } else if (function == "maxinterfaceversion") {
            return plugin.max_interface_version(idam_plugin_interface);
        } else if (function == "openPulse") {
            return plugin.open_pulse(idam_plugin_interface);
        } else if (function == "closePulse") {
            return plugin.close_pulse(idam_plugin_interface);
        } else if (function == "beginAction") {
            return plugin.begin_action(idam_plugin_interface);
        } else if (function == "endAction") {
            return plugin.end_action(idam_plugin_interface);
        } else if (function == "writeData") {
            return plugin.write_data(idam_plugin_interface);
        } else if (function == "readData") {
            return plugin.read_data(idam_plugin_interface);
        } else if (function == "deleteData") {
            return plugin.delete_data(idam_plugin_interface);
        } else if (function == "beginArraystructAction") {
            return plugin.begin_arraystruct_action(idam_plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

namespace {

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int IMASPlugin::help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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
int IMASPlugin::version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int IMASPlugin::build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int IMASPlugin::default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int IMASPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

int IMASPlugin::open_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

int IMASPlugin::close_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

int IMASPlugin::begin_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

#ifdef FIND_REQUIRED_DOUBLE_VALUE
    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);
#else
    float time;
    FIND_REQUIRED_FLOAT_VALUE(request_block->nameValueList, time);
#endif

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    LLenv env = Lowlevel::getLLenv(ctxId);

    PulseContext* pulse_ctx = dynamic_cast<PulseContext*>(env.context);
    OperationContext* op_ctx = new OperationContext(*pulse_ctx, dataObject, access, range, time, interp);

    op_ctx_ = std::unique_ptr<OperationContext>{ op_ctx };

    env.backend->beginAction(op_ctx);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int IMASPlugin::end_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    int type;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, type);

    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->endAction(env.context);

    if (type == CTX_ARRAYSTRUCT_TYPE) {
        array_ctx_stack_.pop();
    } else {
        op_ctx_.reset(nullptr);
    }

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int IMASPlugin::write_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

    PUTDATA_BLOCK* putdata = idam_plugin_interface->request_block->putDataBlockList.putDataBlock;

    Context* ctx = nullptr;
    if (!array_ctx_stack_.empty()) {
        ctx = array_ctx_stack_.top().get();
    } else if (op_ctx_) {
        ctx = op_ctx_.get();
    } else {
        RAISE_PLUGIN_ERROR("no operation or arraystruct context");
    }

    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->writeData(ctx, field, timebase, reinterpret_cast<void*>(const_cast<char*>(putdata->data)),
                           datatype, putdata->rank, putdata->shape);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

int IMASPlugin::read_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

    int rank = 0;
    int dims[64] = {0};

    Context* ctx = nullptr;
    if (!array_ctx_stack_.empty()) {
        ctx = array_ctx_stack_.top().get();
    } else if (op_ctx_) {
        ctx = op_ctx_.get();
    } else {
        RAISE_PLUGIN_ERROR("no operation or arraystruct context");
    }

    env.backend->readData(ctx, field, timebase, &data, &datatype, &rank, dims);

    if (data == nullptr) {
        RAISE_PLUGIN_ERROR("no data returned");
    }

    initDataBlock(idam_plugin_interface->data_block);

    if (rank == 0) {
        if (datatype == CHAR_DATA) {
            setReturnDataString(idam_plugin_interface->data_block, reinterpret_cast<char*>(data), nullptr);
        } else if (datatype == INTEGER_DATA) {
            setReturnDataIntScalar(idam_plugin_interface->data_block, *reinterpret_cast<int*>(data), nullptr);
        } else if (datatype == DOUBLE_DATA) {
            setReturnDataDoubleScalar(idam_plugin_interface->data_block, *reinterpret_cast<double*>(data), nullptr);
        } else {
            RAISE_PLUGIN_ERROR("invalid data type");
        }
    } else {
        std::vector<size_t> shape;
        for (int i = 0; i < rank; ++i) {
            shape.push_back(dims[i]);
        }
        if (datatype == INTEGER_DATA) {
            setReturnDataIntArray(idam_plugin_interface->data_block, reinterpret_cast<int*>(data), rank, shape.data(),
                    nullptr);
        } else if (datatype == DOUBLE_DATA) {
            setReturnDataDoubleArray(idam_plugin_interface->data_block, reinterpret_cast<double*>(data), rank,
                    shape.data(), nullptr);
        } else {
            RAISE_PLUGIN_ERROR("invalid data type");
        }
    }

    return 0;
}

int IMASPlugin::delete_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    int ctxId;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, ctxId);

    const char* path;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    LLenv env = Lowlevel::getLLenv(ctxId);

    env.backend->deleteData(op_ctx_.get(), path);

    setReturnDataIntScalar(idam_plugin_interface->data_block, ctxId, nullptr);
    return 0;
}

bool is_integer(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    char* end = nullptr;
    std::strtol(string.c_str(), &end, 10);
    return end != nullptr && *end == '\0';
}

ArraystructContext* IMASPlugin::build_arraystruct_context(const char* path, const char* timebase)
{
    std::vector<std::string> tokens;

    std::stringstream ss(path);
    std::string fragment;
    while (std::getline(ss, fragment, '/')) {
        tokens.push_back(fragment);
    }

    ArraystructContext* arr_ctx = nullptr;

    for (const auto& token : tokens) {
        if (array_ctx_stack_.empty()) {
            arr_ctx = new ArraystructContext(*op_ctx_, token, timebase, arr_ctx);
        } else if (array_ctx_stack_.top()->getPath() == token) {
            arr_ctx = array_ctx_stack_.top().get();
        } else if (arr_ctx != nullptr) {
            if (is_integer(token)) {
                auto index = static_cast<int>(std::strtol(token.c_str(), nullptr, 10)) - 1;
                arr_ctx->nextIndex(index - arr_ctx->getIndex());
            } else {
                arr_ctx = new ArraystructContext(*op_ctx_, token, timebase, arr_ctx);
            }
        }
    }

    return arr_ctx;
}

int IMASPlugin::begin_arraystruct_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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

#ifdef FIND_REQUIRED_DOUBLE_VALUE
    double time;
    FIND_REQUIRED_DOUBLE_VALUE(request_block->nameValueList, time);
#else
    float time;
    FIND_REQUIRED_FLOAT_VALUE(request_block->nameValueList, time);
#endif

    int interp;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, interp);

    const char* path;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    const char* timebase;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, timebase);

    LLenv env = Lowlevel::getLLenv(ctxId);

    ArraystructContext* array_ctx = build_arraystruct_context(path, timebase);
    
    env.backend->beginArraystructAction(array_ctx, &size);

    array_ctx_stack_.emplace(array_ctx);

    setReturnDataIntScalar(idam_plugin_interface->data_block, size, nullptr);
    return 0;
}

}
