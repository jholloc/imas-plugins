/**
 * Name: IMAS
 *
 * The IMAS plugin is responsible for responding to requests from the IMAS UDA backend.
 * The low level IMAS calls are mapped to plugin functions which are then used
 * to read the data using either a local version of the IMAS backend or another UDA plugin
 * to map non-IMAS data into the IMAS structure.
 */

#include "imas_plugin.h"

#include "uri_parser.h"

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <complex>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <libgen.h>
#include <memory>
#include <stack>
#include <unistd.h>
#include <unordered_map>

#include <clientserver/initStructs.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <serialisation/capnp_serialisation.h>

#include <client/accAPI.h>
#include <client/udaGetAPI.h>
#include <al_lowlevel.h>

#include "machine_mapping.h"

constexpr size_t PATH_LEN = 2048;
constexpr size_t MAX_DIMS = 64;

namespace {

struct Data {
    std::array<char, PATH_LEN> name;
    const unsigned char* data = nullptr;
    int rank = 0;
    std::array<int, MAX_DIMS> dims = {0};
    int datatype = 0;
};

struct DataList {
    Data* list;
    int size;
};

} // namespace

namespace uda {
namespace plugins {
namespace imas {

struct array_structContextCache {
    std::string node;
    int ctx;
    int size;
};

struct OperationContextCache {
    std::string ids;
    int access;
    int range;
    int ctx;
    std::vector<array_structContextCache> array_struct_cache;
};

struct IDSData {
    static constexpr size_t BUFFER_SZ = 8;

    std::string path;
    bool found = false;
    int size = 0;
    bool is_size = false;
    void* data = nullptr;
    std::array<char, BUFFER_SZ> buffer = {};
    bool using_buffer = false;
    int rank = 0;
    std::array<int, MAX_DIMS> shape = {0};
    int datatype = 0;
};

struct Entry {
    bool is_mapped;
    int ctx;
    std::string mapped_machine;
    uri::QueryDict mapped_arguments = {};
    OperationContextCache operation_cache = {"", -1, -1, -1, {}};

    static Entry MappedEntry(std::string mapped_machine, uri::QueryDict mapped_arguments) {
        return {true, -1, std::move(mapped_machine), std::move(mapped_arguments), {}};
    }

    static Entry LocalEntry(int ctx) { return {false, ctx, "", {}, {"", -1, -1, -1, {}}}; }

  private:
    Entry(bool is_mapped, int ctx, std::string&& mapped_machine, uri::QueryDict&& mapped_arguments,
          OperationContextCache&& operation_cache)
        : is_mapped{is_mapped}, ctx(ctx), mapped_machine{std::move(mapped_machine)},
          mapped_arguments{std::move(mapped_arguments)}, operation_cache{std::move(operation_cache)} {}
};

} // namespace imas
} // namespace plugins
} // namespace uda

namespace uda {
namespace plugins {
namespace imas {

using uri_t = std::string;

class Plugin {
  public:
    int init(IDAM_PLUGIN_INTERFACE*) {
        if (_init) {
            return 0;
        }
        _init = true;
        return 0;
    }

    void reset() { _init = false; }

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
    std::unordered_map<uri_t, Entry> _open_entries = {};
    MachineMapping _machine_mapping = {};

    std::vector<IDSData> read_data(Entry& entry, int ctx, std::deque<std::string>& tokens, int datatype, int rank,
                                   const std::string& ids, int is_homogeneous, const std::vector<bool>& dynamic_flags,
                                   const std::string& timebase);

    void read_data_r(Entry& entry, int ctx, std::deque<std::string>& tokens, int datatype, int rank,
                     std::vector<IDSData>& return_data, const std::string& path, int is_homogeneous,
                     const std::vector<bool>& dynamic_flags, const std::string& timebase, int flag_depth);

    std::vector<IDSData> read_mapped_data(const Entry& entry, const std::string& ids,
                                          IDAM_PLUGIN_INTERFACE* plugin_interface, std::deque<std::string>& tokens,
                                          int datatype, int rank);

    void read_mapped_data_r(const Entry& entry, const std::string& ids, IDAM_PLUGIN_INTERFACE* plugin_interface,
                            std::deque<std::string>& tokens, int datatype, int rank, std::vector<IDSData>& return_data,
                            const std::string& path, int flag_depth);

    int get_mapped_data(const Entry& entry, const std::string& ids, IDAM_PLUGIN_INTERFACE* plugin_interface,
                        IDSData& data);
};

} // namespace imas
} // namespace plugins
} // namespace uda

int handle_request(uda::plugins::imas::Plugin& plugin, IDAM_PLUGIN_INTERFACE* plugin_interface) {
    if (plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, nullptr, 10);

    REQUEST_DATA* request_data = plugin_interface->request_data;

    std::string const function = request_data->function;

    if (plugin_interface->housekeeping || function == "reset") {
        plugin.reset();
        return 0;
    }

    plugin.init(plugin_interface);
    if (function == "init") {
        return 0;
    }

    int return_code;

    if (function == "help") {
        return_code = plugin.help(plugin_interface);
    } else if (function == "version") {
        return_code = plugin.version(plugin_interface);
    } else if (function == "builddate") {
        return_code = plugin.build_date(plugin_interface);
    } else if (function == "defaultmethod") {
        return_code = plugin.default_method(plugin_interface);
    } else if (function == "maxinterfaceversion") {
        return_code = plugin.max_interface_version(plugin_interface);
    } else if (function == "get") {
        return_code = plugin.get(plugin_interface);
    } else if (function == "open") {
        return_code = plugin.open(plugin_interface);
    } else if (function == "close") {
        return_code = plugin.close(plugin_interface);
    } else {
        RAISE_PLUGIN_ERROR("Unknown function requested!");
    }

    return return_code;
}

int imasPlugin(IDAM_PLUGIN_INTERFACE* plugin_interface) {
    try {
        static uda::plugins::imas::Plugin plugin{};
        int return_code = handle_request(plugin, plugin_interface);
        concatUdaError(&plugin_interface->error_stack);
        return return_code;
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
int uda::plugins::imas::Plugin::help(IDAM_PLUGIN_INTERFACE* plugin_interface) {
    std::string path = __FILE__;
    std::string const dir = dirname((char*)path.c_str());
    path = dir + "/help.md";
    std::ifstream const ifs(path);
    std::stringstream string_stream;
    string_stream << ifs.rdbuf();

    auto str = string_stream.str();
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
int uda::plugins::imas::Plugin::version(IDAM_PLUGIN_INTERFACE* plugin_interface) {
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
int uda::plugins::imas::Plugin::build_date(IDAM_PLUGIN_INTERFACE* plugin_interface) {
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
int uda::plugins::imas::Plugin::default_method(IDAM_PLUGIN_INTERFACE* plugin_interface) {
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
int uda::plugins::imas::Plugin::max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface) {
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

namespace {

bool is_null_value(void* data, int datatype, int rank) {
    switch (datatype) {
    case CHAR_DATA:
        return data == nullptr || (rank == 0 && *(char*)data == '\0');
    case INTEGER_DATA:
        return data == nullptr || (rank == 0 && *(int*)data == -999999999);
    case DOUBLE_DATA:
        return data == nullptr || (rank == 0 && *(double*)data == -9e+40);
    case COMPLEX_DATA:
        return data == nullptr || (rank == 0 && *(double _Complex*)data == -9e+40);
    default:
        throw std::runtime_error{"unknown datatype"};
    }
}

bool is_index(const std::string& string) {
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

std::pair<std::string, long> parse_index(const std::string& string) {
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
        return {name, -1};
    } else {
        char* end = nullptr;
        long num = std::strtol(rem.c_str(), &end, 10);
        if (*end != '\0') {
            throw std::runtime_error{"invalid string " + string};
        }
        return {name, num};
    }
}

} // namespace

void uda::plugins::imas::Plugin::read_data_r(Entry& entry, int ctx, std::deque<std::string>& tokens, int datatype,
                                             int rank, std::vector<IDSData>& return_data, const std::string& path,
                                             int is_homogeneous, const std::vector<bool>& dynamic_flags,
                                             const std::string& timebase, int depth) {
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
        // handle non-array_struct case

        IDSData data = {};
        data.path = path + "/" + node;
        data.rank = rank;
        data.datatype = datatype;
        for (int i = 0; i < rank; ++i) {
            data.shape[i] = 0;
        }

        if (rank == 0) {
            data.using_buffer = true;
            data.data = (void*)data.buffer.data();
        }

        al_status_t status =
            al_read_data(ctx, node.c_str(), timebase.c_str(), &data.data, datatype, rank, data.shape.data());
        if (status.code != 0) {
            throw std::runtime_error{status.message};
        }

        data.found = !is_null_value(data.data, datatype, rank);
        return_data.push_back(data);
    } else {
        // handle array_struct case

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

        auto& array_struct_cache = entry.operation_cache.array_struct_cache;
        if (array_struct_cache.size() > depth && array_struct_cache[depth].node == node) {
            arr_ctx = array_struct_cache[depth].ctx;
            data.size = array_struct_cache[depth].size;
        } else {
            while (array_struct_cache.size() > depth) {
                al_status_t status = al_end_action(array_struct_cache.back().ctx);
                array_struct_cache.pop_back();
                if (status.code != 0) {
                    throw std::runtime_error{status.message};
                }
            }

            auto is_dynamic = dynamic_flags.at(depth);
            std::string struct_timebase;
            if (is_dynamic) {
                if (is_homogeneous) {
                    struct_timebase = "/time";
                } else if (node == "time") {
                    struct_timebase = "time";
                } else {
                    struct_timebase = node + "/time";
                }
            }

            al_status_t status =
                al_begin_arraystruct_action(ctx, node.c_str(), struct_timebase.c_str(), &data.size, &arr_ctx);
            array_struct_cache.emplace_back(array_structContextCache{node, arr_ctx, data.size});
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
                read_data_r(entry, arr_ctx, copy, datatype, rank, return_data, new_path, is_homogeneous, dynamic_flags,
                            timebase, depth + 1);
                al_iterate_over_arraystruct(arr_ctx, 1);
                ++i;
            }
            al_iterate_over_arraystruct(arr_ctx, -data.size); // reset ctx index back to 0
        } else {
            al_iterate_over_arraystruct(arr_ctx, index);
            std::string new_path = path;
            new_path.append("/").append(node).append("[").append(std::to_string(index)).append("]");
            std::deque<std::string> copy = tokens;
            read_data_r(entry, arr_ctx, copy, datatype, rank, return_data, new_path, is_homogeneous, dynamic_flags,
                        timebase, depth + 1);
            al_iterate_over_arraystruct(arr_ctx, -index); // reset ctx index back to 0
        }

        while (array_struct_cache.size() > depth) {
            al_status_t status = al_end_action(array_struct_cache.back().ctx);
            array_struct_cache.pop_back();
            if (status.code != 0) {
                throw std::runtime_error{status.message};
            }
        }
    }
}

std::vector<uda::plugins::imas::IDSData>
uda::plugins::imas::Plugin::read_data(Entry& entry, int ctx, std::deque<std::string>& tokens, int datatype, int rank,
                                      const std::string& ids, int is_homogeneous,
                                      const std::vector<bool>& dynamic_flags, const std::string& timebase) {
    std::vector<IDSData> return_data;
    read_data_r(entry, ctx, tokens, datatype, rank, return_data, ids, is_homogeneous, dynamic_flags, timebase, 0);

    return return_data;
}

#ifdef _MSC_VER
#define UNREACHABLE() __assume(0)
#else
#define UNREACHABLE() __builtin_unreachable()
#endif

namespace {

UDA_TYPE imas2uda_type(int imas_type) {
    switch (imas_type) {
    case CHAR_DATA:
        return UDA_TYPE_CHAR;
    case INTEGER_DATA:
        return UDA_TYPE_INT;
    case DOUBLE_DATA:
        return UDA_TYPE_DOUBLE;
    case COMPLEX_DATA:
        return UDA_TYPE_COMPLEX;
    default:
        UNREACHABLE();
    }
}

#ifndef MAX_HOST_NAME
#define MAX_HOST_NAME 255
#endif

std::string get_host_name() {
    char host[MAX_HOST_NAME] = {};
#ifdef _WIN32
    DWORD size = MAX_HOST_NAME - 1;
    GetComputerName(host, &size);
#else
    if ((gethostname(host, MAX_HOST_NAME - 1)) != 0) {
        char* env = getenv("HOSTNAME");
        if (env != nullptr)
            copyString(env, host, MAX_HOST_NAME - 1);
    }
#endif
    return host;
}

} // namespace

int uda::plugins::imas::Plugin::get_mapped_data(const Entry& entry, const std::string& ids,
                                                IDAM_PLUGIN_INTERFACE* plugin_interface, IDSData& data) {
    auto plugin = _machine_mapping.plugin(entry.mapped_machine, ids);
    auto host = _machine_mapping.host(entry.mapped_machine, ids);
    auto port = _machine_mapping.port(entry.mapped_machine, ids);

    // Ignore host for now

    std::string shape_string;
    const char* delim = ",";
    for (int i = 0; i < data.rank; ++i) {
        shape_string += (delim + std::to_string(data.shape[i]));
    }

    std::stringstream ss;
    ss << plugin << "::get("
       << "machine='" << entry.mapped_machine << "'"
       << ", path='" << data.path << "'"
       << ", rank=" << data.rank << ", shape=" << shape_string << ", datatype=" << imas2uda_type(data.datatype);

    delim = ", ";
    for (const auto& name : entry.mapped_arguments.names()) {
        if (name == "machine" || name == "path") {
            continue;
        }
        auto value = entry.mapped_arguments.get(name);
        ss << delim << name << "=" << value.value();
    }
    ss << ")";

    std::string const host_name = get_host_name();

    std::string const request = ss.str();
    DATA_BLOCK* data_block = nullptr;

    if (host == "localhost" || host == host_name) {
        int return_code = callPlugin(plugin_interface->pluginList, request.c_str(), plugin_interface);
        if (return_code != 0) {
            return return_code;
        }
        data_block = plugin_interface->data_block;
    } else {
        putIdamServerHost(host.c_str());
        if (port != 0) {
            putIdamServerPort(port);
        }
        int const handle = idamGetAPI(request.c_str(), "");
        if (handle < 0) {
            return handle;
        }
        data_block = getIdamDataBlock(handle);
    }

    if (data_block->data_type == UDA_TYPE_FLOAT && data.datatype == DOUBLE_DATA) {
        auto newdata = reinterpret_cast<double*>(malloc(data_block->data_n * sizeof(double)));
        for (int i = 0; i < data_block->data_n; i++) {
            newdata[i] = static_cast<double>((reinterpret_cast<float*>(data_block->data))[i]);
        }
        free(data_block->data);
        data_block->data = reinterpret_cast<char*>(newdata);
        data_block->data_type = UDA_TYPE_DOUBLE;
    }

    size_t const size_of = getSizeOf((UDA_TYPE)data_block->data_type);
    if (data.rank == 0) {
        data.using_buffer = true;
        data.data = (void*)data.buffer.data();
    } else {
        data.using_buffer = false;
        data.data = malloc(data_block->data_n * size_of);
    }

    for (int i = 0; i < data_block->rank; ++i) {
        data.shape[i] = data_block->dims[i].dim_n;
    }
    memcpy(data.data, data_block->data, data_block->data_n * size_of);
    if (data.is_size) {
        data.size = *reinterpret_cast<int*>(data.data);
    }

    freeDataBlock(data_block);

    return 0;
}

void uda::plugins::imas::Plugin::read_mapped_data_r(const Entry& entry, const std::string& ids,
                                                    IDAM_PLUGIN_INTERFACE* plugin_interface,
                                                    std::deque<std::string>& tokens, int datatype, int rank,
                                                    std::vector<IDSData>& return_data, const std::string& path,
                                                    int depth) {
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
        // handle non-array_struct case

        IDSData data = {};
        data.path = path + "/" + node;
        data.rank = rank;
        data.datatype = datatype;
        for (int i = 0; i < rank; ++i) {
            data.shape[i] = 0;
        }

        int rc = get_mapped_data(entry, ids, plugin_interface, data);

        data.found = (rc == 0) && !is_null_value(data.data, datatype, rank);
        return_data.push_back(data);
    } else {
        // handle array_struct case

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

        IDSData data = {};
        data.found = true;
        data.path = path + "/" + node;
        data.is_size = true;
        data.rank = 0;
        data.datatype = INTEGER_DATA;

        int rc = get_mapped_data(entry, ids, plugin_interface, data);
        if (rc != 0) {
            return;
        }

        return_data.push_back(data);

        if (index == -1) {
            int i = 0;
            while (i < data.size) {
                std::string new_path = path;
                new_path.append("/").append(node).append("[").append(std::to_string(i)).append("]");
                std::deque<std::string> copy = tokens;
                read_mapped_data_r(entry, ids, plugin_interface, copy, datatype, rank, return_data, new_path,
                                   depth + 1);
                ++i;
            }
        } else {
            std::string new_path = path;
            new_path.append("/").append(node).append("[").append(std::to_string(index)).append("]");
            std::deque<std::string> copy = tokens;
            read_mapped_data_r(entry, ids, plugin_interface, copy, datatype, rank, return_data, new_path, depth + 1);
        }
    }
}

std::vector<uda::plugins::imas::IDSData>
uda::plugins::imas::Plugin::read_mapped_data(const Entry& entry, const std::string& ids,
                                             IDAM_PLUGIN_INTERFACE* plugin_interface, std::deque<std::string>& tokens,
                                             int datatype, int rank) {
    std::vector<IDSData> return_data;
    read_mapped_data_r(entry, ids, plugin_interface, tokens, datatype, rank, return_data, ids, 0);

    return return_data;
}

int convert_open_mode(const std::string& mode) {
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

int convert_access_mode(const std::string& access) {
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

int convert_range_mode(const std::string& range) {
    if (range == "global") {
        return GLOBAL_OP;
    } else if (range == "slice") {
        return SLICE_OP;
    } else {
        RAISE_PLUGIN_ERROR("unknown range mode");
    }
}

int convert_interp_mode(const std::string& interp) {
    if (interp == "undefined") {
        return UNDEFINED_INTERP;
    } else if (interp == "closest") {
        return CLOSEST_INTERP;
    } else if (interp == "previous") {
        return PREVIOUS_INTERP;
    } else if (interp == "linear") {
        return LINEAR_INTERP;
    } else {
        RAISE_PLUGIN_ERROR("unknown range mode")
    }
}

int convert_datatype(const std::string& datatype) {
    if (datatype == "char") {
        return CHAR_DATA;
    } else if (datatype == "integer") {
        return INTEGER_DATA;
    } else if (datatype == "double") {
        return DOUBLE_DATA;
    } else if (datatype == "complex") {
        return COMPLEX_DATA;
    } else {
        RAISE_PLUGIN_ERROR("unknown datatype")
    }
}

size_t sizeof_datatype(int type) {
    switch (type) {
    case CHAR_DATA:
        return sizeof(char);
    case INTEGER_DATA:
        return sizeof(int);
    case DOUBLE_DATA:
        return sizeof(double);
    case COMPLEX_DATA:
        return sizeof(std::complex<double>);
    default:
        RAISE_PLUGIN_ERROR("unknown IMAS type")
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
int uda::plugins::imas::Plugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface) {
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

    std::vector<bool> dynamic_flags_vec;
    std::transform(dynamic_flags, dynamic_flags + ndynamic_flags, std::back_inserter(dynamic_flags_vec),
                   [](int flag) { return static_cast<bool>(flag); });

    const char* timebase = nullptr;
    bool const is_timebase = FIND_STRING_VALUE(plugin_interface->request_data->nameValueList, timebase);
    if (!is_timebase) {
        timebase = "";
    }

    if (_open_entries.count(uri) == 0) {
        int const return_code = open(plugin_interface);
        if (return_code != 0) {
            return return_code;
        }
    }

    auto& entry = _open_entries.at(uri);

    std::deque<std::string> tokens;
    boost::split(tokens, path, boost::is_any_of("/"), boost::token_compress_on);

    auto ids = tokens.front();
    tokens.pop_front();
    if (is_integer(tokens.front())) {
        ids += "/" + tokens.front();
        tokens.pop_front();
    }

    std::vector<IDSData> results = {};

    if (entry.is_mapped) {
        try {
            results = read_mapped_data(entry, ids, plugin_interface, tokens, convert_datatype(datatype), rank);
        } catch (std::runtime_error& ex) {
            RAISE_PLUGIN_ERROR(ex.what());
        }
    } else {
        int const access_mode = convert_access_mode(access);
        int const range_mode = convert_range_mode(range);
        int const interp_mode = convert_interp_mode(interp);

        int op_ctx = -1;
        if (entry.operation_cache.ids == ids && entry.operation_cache.access == access_mode &&
            entry.operation_cache.range == range_mode) {
            op_ctx = entry.operation_cache.ctx;
        } else {
            if (entry.operation_cache.ctx != -1) {
                while (!entry.operation_cache.array_struct_cache.empty()) {
                    al_status_t status = al_end_action(entry.operation_cache.array_struct_cache.back().ctx);
                    if (status.code != 0) {
                        RAISE_PLUGIN_ERROR(status.message);
                    }
                    entry.operation_cache.array_struct_cache.pop_back();
                }

                al_status_t status = al_end_action(entry.operation_cache.ctx);
                if (status.code != 0) {
                    RAISE_PLUGIN_ERROR(status.message);
                }
            }

            al_status_t status = {};
            if (range_mode == GLOBAL_OP) {
                status = al_begin_global_action(entry.ctx, ids.c_str(), "", access_mode, &op_ctx);
            } else {
                status = al_begin_slice_action(entry.ctx, ids.c_str(), access_mode, time, interp_mode, &op_ctx);
            }
            if (status.code != 0) {
                RAISE_PLUGIN_ERROR(status.message);
            }
            entry.operation_cache = {ids, access_mode, range_mode, op_ctx, {}};
        }

        try {
            results = read_data(entry, op_ctx, tokens, convert_datatype(datatype), rank, ids, is_homogeneous,
                                dynamic_flags_vec, timebase);
        } catch (std::runtime_error& ex) {
            RAISE_PLUGIN_ERROR(ex.what())
        }
    }

    if (results.empty()) {
        initDataBlock(plugin_interface->data_block);
        return 0;
    }

    std::vector<const IDSData*> found_results = {};
    for (const auto& result : results) {
        if (!result.found) {
            continue;
        }
        found_results.push_back(&result);
    }

    auto* tree = uda_capnp_new_tree();
    auto* root = uda_capnp_get_root(tree);
    uda_capnp_set_node_name(root, "root");
    uda_capnp_add_children(root, found_results.size());

    size_t index = 0;
    for (const auto* const result : found_results) {
        auto* child = uda_capnp_get_child(tree, root, index);

        uda_capnp_set_node_name(child, result->path.c_str());

        uda_capnp_add_children(child, 2);
        auto* shape_node = uda_capnp_get_child(tree, child, 0);
        uda_capnp_set_node_name(shape_node, "shape");
        auto* data_node = uda_capnp_get_child(tree, child, 1);
        uda_capnp_set_node_name(data_node, "data");

        uda_capnp_add_array_i32(shape_node, result->shape.data(), result->rank);

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
                    uda_capnp_add_i32(data_node, *reinterpret_cast<const int32_t*>(result->buffer.data()));
                } else {
                    uda_capnp_add_array_i32(data_node, reinterpret_cast<int32_t*>(result->data), count);
                }
                break;
            case DOUBLE_DATA:
                if (result->using_buffer) {
                    uda_capnp_add_f64(data_node, *reinterpret_cast<const double*>(result->buffer.data()));
                } else {
                    uda_capnp_add_array_f64(data_node, reinterpret_cast<double*>(result->data), count);
                }
                break;
            case CHAR_DATA:
                if (result->using_buffer) {
                    uda_capnp_add_char(data_node, *reinterpret_cast<const char*>(result->buffer.data()));
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
int uda::plugins::imas::Plugin::open(IDAM_PLUGIN_INTERFACE* plugin_interface) {
    const char* uri;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, uri);

    const char* mode;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, mode);

    int mode_int = convert_open_mode(mode);

    auto parsed_uri = uri::parse_uri(uri);
    auto maybe_machine = parsed_uri.query.get("machine");
    if (maybe_machine) {
        auto mapped_machine = maybe_machine.value();

        if (!_machine_mapping.contains(mapped_machine)) {
            RAISE_PLUGIN_ERROR("machine not found")
        }

        _open_entries.emplace(uri, Entry::MappedEntry(mapped_machine, parsed_uri.query));
        return setReturnDataIntScalar(plugin_interface->data_block, 0, "mapping return");
    }

    int ctx;
    al_status_t status = al_begin_dataentry_action(uri, mode_int, &ctx);
    if (status.code != 0) {
        std::string msg = std::string{"failed to open pulse: "} + status.message;
        RAISE_PLUGIN_ERROR(msg.c_str());
    }

    initDataBlock(plugin_interface->data_block);

    _open_entries.emplace(uri, Entry::LocalEntry(ctx));

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
int uda::plugins::imas::Plugin::close(IDAM_PLUGIN_INTERFACE* plugin_interface) {
    const char* uri;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, uri);

    int mode;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, mode);

    initDataBlock(plugin_interface->data_block);

    if (_open_entries.count(uri) == 0) {
        RAISE_PLUGIN_ERROR("pulse is not currently open");
    }
    auto& entry = _open_entries.at(uri);

    if (entry.is_mapped) {
        _open_entries.erase(uri);
        return setReturnDataIntScalar(plugin_interface->data_block, 0, "mapped return");
    }

    if (entry.operation_cache.ctx != -1) {
        while (!entry.operation_cache.array_struct_cache.empty()) {
            al_status_t status = al_end_action(entry.operation_cache.array_struct_cache.back().ctx);
            if (status.code != 0) {
                RAISE_PLUGIN_ERROR(status.message);
            }
            entry.operation_cache.array_struct_cache.pop_back();
        }

        al_status_t status = al_end_action(entry.operation_cache.ctx);
        if (status.code != 0) {
            RAISE_PLUGIN_ERROR(status.message);
        }
        entry.operation_cache = {"", -1, -1, {}};
    }

    al_status_t status = al_close_pulse(entry.ctx, mode);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }

    status = al_end_action(entry.ctx);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }
    _open_entries.erase(uri);

    return setReturnDataIntScalar(plugin_interface->data_block, -1, "pulse context");
}
