/**
 * Name: IMAS
 *
 * The IMAS plugin is responsible for responding to requests from the IMAS UDA backend.
 * The low level IMAS calls are mapped to plugin functions which are then used
 * to read the data using either a local version of the IMAS backend or another UDA plugin
 * to map non-IMAS data into the IMAS structure.
 */

#include "imas_plugin.h"

#include <memory>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string.hpp>
#include <libgen.h>
#include <complex.h>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <ual_lowlevel.h>
#include <structures/struct.h>
#include <structures/accessors.h>
#include <serialisation/capnp_serialisation.h>

#define PATH_LEN 2048

namespace {

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

}

namespace uda {
namespace plugins {
namespace imas {

struct ArraystructContextCache
{
    std::string node;
    int ctx;
    int size;
};

struct OperationContextCache
{
    std::string ids;
    int access;
    int ctx;
    std::vector<ArraystructContextCache> arraystruct_cache;
};

struct IDSData
{
    std::string path;
    bool found = false;
    int size = 0;
    bool is_size = false;
    void* data = nullptr;
    char buffer[8] = {};
    bool using_buffer = false;
    int rank = 0;
    int shape[64] = {0};
    int datatype = 0;
};

}
}
}

namespace uda {
namespace plugins {
namespace imas {

using uri_t = std::string;

class Plugin
{
public:
    int init(IDAM_PLUGIN_INTERFACE* plugin_interface)
    {
        if (_init) {
            return 0;
        }
        _init = true;
        return 0;
    }

    void reset()
    {
        _init = false;
    }

    int help(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int version(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int build_date(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int default_method(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int get(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int open(IDAM_PLUGIN_INTERFACE* plugin_interface);

    int close(IDAM_PLUGIN_INTERFACE* plugin_interface);

private:
    bool _init = false;
    std::unordered_map<uri_t, int> _open_entries = {};
    OperationContextCache _operation_cache = { "", -1, -1, {} };

    std::vector<IDSData>
    read_data(int ctx, std::deque<std::string>& tokens, int datatype, int rank, const std::string& ids,
              int is_homogeneous, const std::vector<int>& dynamic_flags);

    void read_data_r(int ctx, std::deque<std::string>& tokens, int datatype, int rank,
                     std::vector<IDSData>& return_data, const std::string& path, int is_homogeneous,
                     const std::vector<int>& dynamic_flags, int flag_depth);
};

}
}
}

int handle_request(uda::plugins::imas::Plugin& plugin, IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    if (plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, nullptr, 10);

    REQUEST_DATA* request_data = plugin_interface->request_data;

    std::string function = request_data->function;

    if (plugin_interface->housekeeping || function == "reset") {
        plugin.reset();
        return 0;
    }

    plugin.init(plugin_interface);
    if (function == "init") {
        return 0;
    }

    if (function == "help") {
        return plugin.help(plugin_interface);
    } else if (function == "version") {
        return plugin.version(plugin_interface);
    } else if (function == "builddate") {
        return plugin.build_date(plugin_interface);
    } else if (function == "defaultmethod") {
        return plugin.default_method(plugin_interface);
    } else if (function == "maxinterfaceversion") {
        return plugin.max_interface_version(plugin_interface);
    } else if (function == "get") {
        return plugin.get(plugin_interface);
    } else if (function == "open") {
        return plugin.open(plugin_interface);
    } else if (function == "close") {
        return plugin.close(plugin_interface);
    } else {
        RAISE_PLUGIN_ERROR("Unknown function requested!");
    }
}

int imasPlugin(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    try {
        static uda::plugins::imas::Plugin plugin{};
        int rc = handle_request(plugin, plugin_interface);
        concatUdaError(&plugin_interface->error_stack);
        return rc;
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR_EX(ex.what(), { concatUdaError(&plugin_interface->error_stack); });
    }
}

/**
 * Function: help
 *
 * The `help` function returns the help documentation for this plugin.
 *
 * Returns:
 *      Signal[datatype=STRING, rank=0]
 *
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::help(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    std::string path = __FILE__;
    std::string dir = dirname((char*)path.c_str());
    path = dir + "/help.md";
    std::ifstream ifs(path);
    std::stringstream ss;
    ss << ifs.rdbuf();

    auto str = ss.str();
    const char* help = str.c_str();
    const char* desc = PLUGIN_NAME ": help = description of this plugin";

    return setReturnDataString(plugin_interface->data_block, help, desc);
}

/**
 * Function: version
 *
 * The `version` function returns the version of the plugin being called.
 *
 * Returns:
 *      Signal[datatype=int, rank=0]
 *
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, PLUGIN_VERSION, "Plugin version number");
}

/**
 * Function: buildDate
 *
 * The `buildDate` function returns the date this plugin was built.
 *
 * Returns:
 *      Signal[datatype=STRING, rank=0]
 *
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::build_date(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Function: defaultMethod
 *
 * The `defaultMethod` function returns the method that will be called if no function name is provided.
 *
 * Returns:
 *      Signal[datatype=STRING, rank=0]
 *
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::default_method(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Function: maxInterfaceVersion
 *
 * The `maxInterfaceVersion` function returns the highest UDA plugin interface version that this plugin will respond to.
 *
 * Returns:
 *      Signal[datatype=int, rank=0]
 *
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

namespace {

bool is_null_value(void* data, int datatype)
{
    switch (datatype) {
        case CHAR_DATA:
            return data == nullptr || *(char*)data == '\0';
        case INTEGER_DATA:
            return data == nullptr || *(int*)data == -999999999;
        case DOUBLE_DATA:
            return data == nullptr || *(double*)data == -9e+40;
        case COMPLEX_DATA:
            return data == nullptr || *(double _Complex*)data == -9e+40;
        default:
            throw std::runtime_error{"unknown datatype"};
    }
}

bool is_index(const std::string& string)
{
    if (string.back() != ']') {
        return false;
    }

    auto pos = string.find('[');

    if (pos == std::string::npos || pos == 0) {
        return false;
    }

    std::string rem = std::string{string.begin() + pos + 1, string.end() - 1};
    if (rem.empty()) {
        return false;
    }

    char* end = nullptr;
    std::strtol(rem.c_str(), &end, 10);

    return rem == ":" || *end == '\0';
}

bool is_integer(const std::string& string) {
    char* end = nullptr;
    std::strtol(string.c_str(), &end, 10);
    return *end == '\0';
}

std::pair<std::string, long> parse_index(const std::string& string)
{
    if (string.back() != ']') {
        throw std::runtime_error{"invalid string " + string};
    }

    auto pos = string.find('[');

    if (pos == std::string::npos || pos == 0) {
        throw std::runtime_error{"invalid string " + string};
    }

    std::string name = std::string{string.begin(), string.begin() + pos};
    std::string rem = std::string{string.begin() + pos + 1, string.end() - 1};

    if (rem == ":") {
        return { name, -1 };
    } else {
        char* end = nullptr;
        long num = std::strtol(rem.c_str(), &end, 10);
        if (*end != '\0') {
            throw std::runtime_error{"invalid string " + string};
        }
        return { name, num };
    }
}

} // anon namespace

void
uda::plugins::imas::Plugin::read_data_r(int ctx, std::deque<std::string>& tokens,
                                        int datatype, int rank, std::vector<IDSData>& return_data,
                                        const std::string& path, int is_homogeneous,
                                        const std::vector<int>& dynamic_flags, int depth)
{
    if (tokens.empty()) {
        return;
    }

    std::string node;
    std::string delim;

    while (!tokens.empty() && !is_index(tokens.front())) {
        node += delim + tokens.front();
        tokens.pop_front();

        delim = "/";
    }

    if (tokens.empty()) {
        // handle non-arraystruct case

        IDSData data = {};
        data.path = path + "/" + node;
        data.rank = rank;
        data.datatype = datatype;
        for (int i = 0; i < rank; ++i) {
            data.shape[i] = 0;
        }

        if (rank == 0) {
            data.using_buffer = true;
            data.data = (void*)data.buffer;
        }

        const char* time = "";
        al_status_t status = ual_read_data(ctx, node.c_str(), time, &data.data, datatype, rank, data.shape);
        if (status.code != 0) {
            throw std::runtime_error{status.message};
        }

        data.found = !is_null_value(data.data, datatype);
        return_data.push_back(data);
    } else {
        // handle arraystruct case

        assert(is_index(tokens.front()));

        auto head = tokens.front();
        tokens.pop_front();

        auto pair = parse_index(head);
        if (!node.empty()) {
            node = node + "/" + pair.first;
        } else {
            node = pair.first;
        }
        long index = pair.second;

        int arr_ctx;
        IDSData data = {};
        data.found = true;
        data.path = path + "/" + node;
        data.is_size = true;
        data.rank = 0;
        data.datatype = INTEGER_DATA;

        auto& arraystruct_cache = _operation_cache.arraystruct_cache;
        if (arraystruct_cache.size() > depth && arraystruct_cache[depth].node == node) {
            arr_ctx = arraystruct_cache[depth].ctx;
            data.size = arraystruct_cache[depth].size;
        } else {
            while (arraystruct_cache.size() > depth) {
                al_status_t status = ual_end_action(arraystruct_cache.back().ctx);
                arraystruct_cache.pop_back();
                if (status.code != 0) {
                    throw std::runtime_error{status.message};
                }
            }

            auto is_dynamic = dynamic_flags.at(depth);
            std::string timebase;
            if (is_dynamic) {
                if (is_homogeneous) {
                    timebase = "/time";
                } else {
                    timebase = node + "/time";
                }
            }

            al_status_t status = ual_begin_arraystruct_action(ctx, node.c_str(), timebase.c_str(), &data.size, &arr_ctx);
            arraystruct_cache.emplace_back(ArraystructContextCache{ node, arr_ctx, data.size });
            if (status.code != 0) {
                throw std::runtime_error{status.message};
            }
        }

        return_data.push_back(data);

        if (index == -1) {
            int i = 0;
            while (i < data.size) {
                std::string new_path = path;
                new_path.append("/").append(node).append("[").append(std::to_string(i)).append("]");
                std::deque<std::string> copy = tokens;
                read_data_r(arr_ctx, copy, datatype, rank, return_data, new_path, is_homogeneous, dynamic_flags, depth + 1);
                ual_iterate_over_arraystruct(arr_ctx, 1);
                ++i;
            }
            ual_iterate_over_arraystruct(arr_ctx, -data.size); // reset ctx index back to 0
        } else {
            ual_iterate_over_arraystruct(arr_ctx, index);
            std::string new_path = path;
            new_path.append("/").append(node).append("[").append(std::to_string(index)).append("]");
            std::deque<std::string> copy = tokens;
            read_data_r(arr_ctx, copy, datatype, rank, return_data, new_path, is_homogeneous, dynamic_flags, depth + 1);
            ual_iterate_over_arraystruct(arr_ctx, -index); // reset ctx index back to 0
        }

        while (arraystruct_cache.size() > depth) {
            al_status_t status = ual_end_action(arraystruct_cache.back().ctx);
            arraystruct_cache.pop_back();
            if (status.code != 0) {
                throw std::runtime_error{status.message};
            }
        }
    }
}

std::vector<uda::plugins::imas::IDSData>
uda::plugins::imas::Plugin::read_data(int ctx, std::deque<std::string>& tokens, int datatype, int rank,
                                      const std::string& ids, int is_homogeneous, const std::vector<int>& dynamic_flags)
{
    std::vector<IDSData> return_data;
    read_data_r(ctx, tokens, datatype, rank, return_data, ids, is_homogeneous, dynamic_flags, 0);

    return return_data;
}

int convert_open_mode(const std::string& mode)
{
    if (mode == "open") {
        return OPEN_PULSE;
    } else if (mode == "create") {
        return CREATE_PULSE;
    } else if (mode == "force_open") {
        return FORCE_OPEN_PULSE;
    } else if (mode == "force_create") {
        return FORCE_CREATE_PULSE;
    } else {
        RAISE_PLUGIN_ERROR("unknown open mode");
    }
}

int convert_access_mode(const std::string& access)
{
    if (access == "read") {
        return READ_OP;
    } else if (access == "write") {
        return WRITE_OP;
    } else if (access == "replace") {
        return REPLACE_OP;
    } else {
        RAISE_PLUGIN_ERROR("unknown access mode");
    }
}

int convert_datatype(const std::string& datatype)
{
    if (datatype == "char") {
        return CHAR_DATA;
    } else if (datatype == "integer") {
        return INTEGER_DATA;
    } else if (datatype == "double") {
        return DOUBLE_DATA;
    } else if (datatype == "complex") {
        return COMPLEX_DATA;
    } else {
        RAISE_PLUGIN_ERROR("unknown datatype");
    }
}

size_t sizeof_datatype(int type)
{
    switch (type) {
        case CHAR_DATA: return sizeof(char);
        case INTEGER_DATA: return sizeof(int);
        case DOUBLE_DATA: return sizeof(double);
        case COMPLEX_DATA: return sizeof(std::complex<double>);
        default: RAISE_PLUGIN_ERROR("unknown IMAS type");
    }
}

/**
 * Function: get
 *
 * Returns the IMAS data for the given IDS path. If the database entry is not currently open then it will be opened.
 *
 * Arguments:
 *      uri             (required, string)  - uri for data
 *      access          (required, string)  - read access mode `[read|write|replace]`
 *      range           (required, string)  - range mode `[global|slice]`
 *      time            (required, float)   - slice time (ignored for global range mode)
 *      interp          (required, string)  - interpolation mode (ignored for global range mode)
 *      path            (required, string)  - IDS path, i.e. `flux_loop[3]/flux/data`
 *      datatype        (required, string)  - IDS data type `[char|integer|double|complex]`
 *      rank            (required, int)     - rank of data to return
 *      is_homogeneous  (required, int)     - flag specifying whether data has been stored homogeneously
 *      dynamic_flags   (required, int array) - flags specifying dynamic status for each level of the path
 *
 * Returns:
 *      CapNp serialised tree of depth 1, where each leaf node contains the name and data of a returned IMAS data node
 *
 * @param plugin_interface the UDA plugin interface structure
 * @return 0 on success, !=0 on error
 */
int uda::plugins::imas::Plugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    const char* uri;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, uri);

    const char* access = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, access);

    const char* range = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, range);

    float time = 0.0;
    FIND_REQUIRED_FLOAT_VALUE(plugin_interface->request_data->nameValueList, time);

    const char* interp = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, interp);

    const char* path = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, path);

    const char* datatype = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, datatype);

    int rank = -1;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, rank);

    int is_homogeneous = -1;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, is_homogeneous);

    int* dynamic_flags = nullptr;
    size_t ndynamic_flags = -1;
    FIND_REQUIRED_INT_ARRAY(plugin_interface->request_data->nameValueList, dynamic_flags);

    std::vector<int> dynamic_flags_vec = { dynamic_flags, dynamic_flags + ndynamic_flags };

    if (!_open_entries.count(uri)) {
        int rc = open(plugin_interface);
        if (rc != 0) {
            return rc;
        }
    }
    int ctxId = _open_entries[uri];

    std::deque<std::string> tokens;
    boost::split(tokens, path, boost::is_any_of("/"), boost::token_compress_on);

    auto ids = tokens.front();
    tokens.pop_front();
    if (is_integer(tokens.front())) {
        ids += "/" + tokens.front();
        tokens.pop_front();
    }

    std::string s_access = access;

    int iaccess = convert_access_mode(access);

    int op_ctx = -1;
    if (_operation_cache.ids == ids && _operation_cache.access == iaccess) {
        op_ctx = _operation_cache.ctx;
    } else {
        if (_operation_cache.ctx != -1) {
            while (!_operation_cache.arraystruct_cache.empty()) {
                al_status_t status = ual_end_action(_operation_cache.arraystruct_cache.back().ctx);
                if (status.code != 0) {
                    RAISE_PLUGIN_ERROR(status.message);
                }
                _operation_cache.arraystruct_cache.pop_back();
            }

            al_status_t status = ual_end_action(_operation_cache.ctx);
            if (status.code != 0) {
                RAISE_PLUGIN_ERROR(status.message);
            }
        }

        al_status_t status = ual_begin_global_action(ctxId, ids.c_str(), "", iaccess, &op_ctx);
        if (status.code != 0) {
            RAISE_PLUGIN_ERROR(status.message);
        }
        _operation_cache = { ids, iaccess, op_ctx, {} };
    }

    std::vector<IDSData> results = {};
    try {
        results = read_data(op_ctx, tokens, convert_datatype(datatype), rank, ids, is_homogeneous, dynamic_flags_vec);
        if (results.empty()) {
            initDataBlock(plugin_interface->data_block);
            return 0;
        }
    } catch (std::runtime_error& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }

    std::vector<const IDSData*> found_results = {};
    for (const auto& result: results) {
        if (!result.found) {
            continue;
        }
        found_results.push_back(&result);
    }

    auto tree = uda_capnp_new_tree();
    auto root = uda_capnp_get_root(tree);
    uda_capnp_set_node_name(root, "root");
    uda_capnp_add_children(root, found_results.size());

    size_t index = 0;
    for (const auto result: found_results) {
        auto child = uda_capnp_get_child(tree, root, index);

        uda_capnp_set_node_name(child, result->path.c_str());

        uda_capnp_add_children(child, 2);
        auto shape_node = uda_capnp_get_child(tree, child, 0);
        uda_capnp_set_node_name(shape_node, "shape");
        auto data_node = uda_capnp_get_child(tree, child, 1);
        uda_capnp_set_node_name(data_node, "data");

        uda_capnp_add_array_i32(shape_node, result->shape, result->rank);

        if (result->is_size) {
            uda_capnp_add_i32(data_node, result->size);
        } else {
            size_t count = 1;
            for (int i = 0; i < result->rank; ++i) {
                count *= result->shape[i];
            }

            if (result->using_buffer && count > 1) {
                RAISE_PLUGIN_ERROR("too much data to read from result buffer");
            }

            switch (result->datatype) {
                case INTEGER_DATA:
                    if (result->using_buffer) {
                        uda_capnp_add_i32(data_node, *reinterpret_cast<const int32_t*>(result->buffer));
                    } else {
                        uda_capnp_add_array_i32(data_node, reinterpret_cast<int32_t*>(result->data), count);
                    }
                    break;
                case DOUBLE_DATA:
                    if (result->using_buffer) {
                        uda_capnp_add_f64(data_node, *reinterpret_cast<const double*>(result->buffer));
                    } else {
                        uda_capnp_add_array_f64(data_node, reinterpret_cast<double*>(result->data), count);
                    }
                    break;
                case CHAR_DATA:
                    if (result->using_buffer) {
                        uda_capnp_add_char(data_node, *reinterpret_cast<const char*>(result->buffer));
                    } else {
                        uda_capnp_add_array_char(data_node, reinterpret_cast<char*>(result->data), count);
                    }
                    break;
                default:
                    break;
            }
        }

        ++index;
    }

    auto buffer = uda_capnp_serialise(tree);

    DATA_BLOCK* data_block = plugin_interface->data_block;
    initDataBlock(data_block);

    data_block->data_n = static_cast<int>(buffer.size);
    data_block->data = buffer.data;
    data_block->dims = nullptr;
    data_block->data_type = UDA_TYPE_CAPNP;

    return 0;
}

/**
 * Function: open
 *
 * Open an IDS database entry, caching the pulse context handle.
 *
 * Arguments:
 *      uri     (required, string)  - uri for data
 *      mode    (required, string)  - open mode `[open|create|force_open|force_create]`
 *
 * Returns:
 *      Integer scalar containing the pulse context handle.
 *
 * Example:
 *      IMAS::open(shot=1000, run=1, user='test', tokamak='iter', version='3')
 *
 * @param plugin_interface the UDA plugin interface structure
 * @return 0 on success, !=0 on error
 */
int uda::plugins::imas::Plugin::open(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    const char* uri;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, uri);

    const char* mode;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, mode);

    int mode_int = convert_open_mode(mode);

    int ctx;
    al_status_t status = ual_begin_dataentry_action(uri, mode_int, &ctx);
    if (status.code != 0) {
        std::string msg = std::string{ "failed to open pulse: " } + status.message;
        RAISE_PLUGIN_ERROR(msg.c_str());
    }

    initDataBlock(plugin_interface->data_block);

    _open_entries[uri] = ctx;

    return setReturnDataIntScalar(plugin_interface->data_block, ctx, "pulse context");
}

/**
 * Function: close
 *
 * Closes the IMAS database entry corresponding to the given arguments. The entry must have been opened by calling the
 * open(...) or get(...) functions.
 *
 * Arguments:
 *      uri     (required, string)  - uri for data
 *      mode    (required, int)     - close mode
 *
 * Returns:
 *      Integer scalar -1
 *
 * Example:
 *      IMAS::close(shot=1000, run=1, user='test', tokamak='iter', version='3')
 *
 * @param plugin_interface the UDA plugin interface structure
 * @return 0 on success, !=0 on error
 */
int uda::plugins::imas::Plugin::close(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    const char* uri;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, uri);

    int mode;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, mode);

    initDataBlock(plugin_interface->data_block);

    if (_open_entries.count(uri) == 0) {
        RAISE_PLUGIN_ERROR("pulse is not currently open");
    }
    int ctx = _open_entries[uri];

    if (_operation_cache.ctx != -1) {
        while (!_operation_cache.arraystruct_cache.empty()) {
            al_status_t status = ual_end_action(_operation_cache.arraystruct_cache.back().ctx);
            if (status.code != 0) {
                RAISE_PLUGIN_ERROR(status.message);
            }
            _operation_cache.arraystruct_cache.pop_back();
        }

        al_status_t status = ual_end_action(_operation_cache.ctx);
        if (status.code != 0) {
            RAISE_PLUGIN_ERROR(status.message);
        }
        _operation_cache = { "", -1, -1, {} };
    }

    al_status_t status = ual_close_pulse(ctx, mode, "");
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }

    status = ual_end_action(ctx);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }
    _open_entries.erase(uri);

    return setReturnDataIntScalar(plugin_interface->data_block, -1, "pulse context");
}
