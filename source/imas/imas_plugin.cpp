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

namespace {

void define_usertypes(USERDEFINEDTYPELIST* user_defined_type_list)
{
    USERDEFINEDTYPE data_usertype;
    initUserDefinedType(&data_usertype);

    strcpy(data_usertype.name, "Data");
    strcpy(data_usertype.source, "IMAS");
    data_usertype.ref_id = 0;
    data_usertype.imagecount = 0; // No Structure Image data
    data_usertype.image = nullptr;
    data_usertype.size = sizeof(Data); // Structure size
    data_usertype.idamclass = UDA_TYPE_COMPOUND;

    int name_shape[] = {PATH_LEN};
    ::addStructureField(&data_usertype, "name", "name", UDA_TYPE_STRING, false, 1, name_shape, offsetof(Data, name));
    ::addStructureField(&data_usertype, "value", "data pointer", UDA_TYPE_UNSIGNED_CHAR, true, 0, nullptr,
                        offsetof(Data, data));
    ::addStructureField(&data_usertype, "rank", "data rank", UDA_TYPE_INT, false, 0, nullptr, offsetof(Data, rank));
    int dims_shape[] = {64};
    ::addStructureField(&data_usertype, "dims", "data dimensions", UDA_TYPE_INT, false, 1, dims_shape,
                        offsetof(Data, dims));
    ::addStructureField(&data_usertype, "datatype", "data type", UDA_TYPE_INT, false, 0, nullptr,
                        offsetof(Data, datatype));

    addUserDefinedType(user_defined_type_list, data_usertype);

    USERDEFINEDTYPE data_list_usertype;
    initUserDefinedType(&data_list_usertype);

    strcpy(data_list_usertype.name, "DataList");
    strcpy(data_list_usertype.source, "IMAS");
    data_list_usertype.ref_id = 0;
    data_list_usertype.imagecount = 0; // No Structure Image data
    data_list_usertype.image = nullptr;
    data_list_usertype.size = sizeof(DataList);  // Structure size
    data_list_usertype.idamclass = UDA_TYPE_COMPOUND;

    COMPOUNDFIELD list_field;
    initCompoundField(&list_field);

    strcpy(list_field.name, "list");
    list_field.atomictype = UDA_TYPE_UNKNOWN;
    strcpy(list_field.type, "Data");
    strcpy(list_field.desc, "list of data");
    list_field.pointer = 1;
    list_field.count = 1;
    list_field.rank = 0;
    list_field.shape = nullptr;
    list_field.size = sizeof(Data*);
    list_field.offset = (int)offsetof(DataList, list);
    list_field.offpad = (int)padding(offsetof(DataList, list), list_field.type);
    list_field.alignment = getalignmentof(list_field.type);
    addCompoundField(&data_list_usertype, list_field);

//    ::addStructureField(&data_list_usertype, "list", "list of data", UDA_TYPE_UNKNOWN, true, 0, nullptr, offsetof(DataList, list));
    ::addStructureField(&data_list_usertype, "size", "number of data", UDA_TYPE_INT, false, 0, nullptr, offsetof(DataList, size));

//    COMPOUNDFIELD size_field;
//    initCompoundField(&size_field);
//
//    int offset = (int)offsetof(DataList, size);
//    defineField(&size_field, "size", "number of data", &offset, SCALARINT);
//    addCompoundField(&data_list_usertype, size_field);

    addUserDefinedType(user_defined_type_list, data_list_usertype);
}

}

namespace uda {
namespace plugins {
namespace imas {

struct Entry
{
    int shot;
    int run;
    std::string user;
    std::string tokamak;
    std::string version;

    bool operator==(const Entry& other) const
    {
        return shot == other.shot && run == other.run && user == other.user && tokamak == other.tokamak &&
               version == other.version;
    }
};

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

template<>
struct std::hash<uda::plugins::imas::Entry>
{
    std::size_t operator()(uda::plugins::imas::Entry const& entry) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, entry.shot);
        boost::hash_combine(seed, entry.run);
        boost::hash_combine(seed, entry.user);
        boost::hash_combine(seed, entry.tokamak);
        boost::hash_combine(seed, entry.version);
        return seed;
    }
};

namespace uda {
namespace plugins {
namespace imas {

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
    std::unordered_map<Entry, int> _open_entries = {};
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
 * Help: A Description of library functionality
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
 * Plugin version
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, PLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::build_date(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::default_method(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param plugin_interface
 * @return
 */
int uda::plugins::imas::Plugin::max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

bool is_null_value(void* data, int datatype)
{
    switch (datatype) {
        case CHAR_DATA: return data == nullptr || *(char*)data == '\0';
        case INTEGER_DATA: return data == nullptr || *(int*)data == -999999999;
        case DOUBLE_DATA: return data == nullptr || *(double*)data == -9e+40;
        case COMPLEX_DATA: return data == nullptr || *(double _Complex*)data == -9e+40;
        default: throw std::runtime_error{"unknown datatype"};
    }
}

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

    while (!tokens.empty() && !boost::ends_with(tokens.front(), "[:]")) {
        node += delim + tokens.front();
        tokens.pop_front();

        delim = "/";
    }

    if (!node.empty()) {
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
    } else if (!tokens.empty()) {
        // handle arraystruct case

        assert(boost::ends_with(tokens.front(), "[:]"));

        auto head = tokens.front();
        tokens.pop_front();

        node = head.substr(0, head.size() - 3);

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
            ual_iterate_over_arraystruct(arr_ctx, -data.size);
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

        int i = 0;
        while (i < data.size) {
            std::string new_path = path;
            new_path.append("/").append(node).append("[").append(std::to_string(i)).append("]");
            std::deque<std::string> copy = tokens;
            read_data_r(arr_ctx, copy, datatype, rank, return_data, new_path, is_homogeneous, dynamic_flags, depth + 1);
            ual_iterate_over_arraystruct(arr_ctx, 1);
            ++i;
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

int convert_access_mode(const std::string& access)
{
    int n = -1;
    if (access == "read") {
        n = READ_OP;
    } else if (access == "write") {
        n = WRITE_OP;
    } else if (access == "replace") {
        n = REPLACE_OP;
    } else {
        RAISE_PLUGIN_ERROR("unknown access mode");
    }
    return n;
}

int convert_datatype(const std::string& datatype)
{
    int n = -1;
    if (datatype == "char") {
        n = CHAR_DATA;
    } else if (datatype == "integer") {
        n = INTEGER_DATA;
    } else if (datatype == "double") {
        n = DOUBLE_DATA;
    } else if (datatype == "complex") {
        n = COMPLEX_DATA;
    } else {
        RAISE_PLUGIN_ERROR("unknown datatype");
    }
    return n;
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
 * Returns the IMAS data for the given IDS path. If the database entry is not currently open then it will be opened.
 *
 * Arguments:
 *      shot            (required, int)     - shot number
 *      run             (required, int)     - run number
 *      user            (required, string)  - user or path
 *      tokamak         (required, string)  - tokamak name
 *      version         (required, string)  - IMAS version
 *      dataObject      (required, string)  - IDS name, i.e. magnetics, equilibrium, etc.
 *      access          (required, string)  - read access mode [read|write|replace]
 *      range           (required, string)  - range mode [global|slice]
 *      time            (required, float)   - slice time (ignored for global range mode)
 *      interp          (required, string)  - interpolation mode (ignored for global range mode)
 *      path            (required, string)  - IDS path, i.e. flux_loop[3]/flux/data
 *      datatype        (required, string)  - IDS data type [char|integer|double|complex]
 *      rank            (required, int)     - rank of data to return
 *      is_homogeneous  (required, int)     - flag specifying whether data has been stored homogeneously
 *      dynamic_flags   (required, int array) - flags specifying dynamic status for each level of the path
 *
 * @param plugin_interface the UDA plugin interface structure
 * @return 0 on success, !=0 on error
 */
int uda::plugins::imas::Plugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    int shot;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, user);

    const char* tokamak;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, tokamak);

    const char* version;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, version);

    const char* dataObject = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, dataObject);

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

    Entry entry = {shot, run, user, tokamak, version};
    if (!_open_entries.count(entry)) {
        int rc = open(plugin_interface);
        if (rc != 0) {
            return rc;
        }
    }
    int ctxId = _open_entries[entry];

    std::deque<std::string> tokens;
    boost::split(tokens, path, boost::is_any_of("/"), boost::token_compress_on);

    auto ids = tokens.front();
    tokens.pop_front();

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

        al_status_t status = ual_begin_global_action(ctxId, ids.c_str(), iaccess, &op_ctx);
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

//    status = ual_end_action(ctxId);
//    if (status.code != 0) {
//        RAISE_PLUGIN_ERROR(status.message);
//    }
    define_usertypes(plugin_interface->userdefinedtypelist);

    Data* data = (Data*)calloc(results.size(), sizeof(Data));
    addMalloc(plugin_interface->logmalloclist, (void*)data, (int)results.size(), sizeof(Data), "Data");

    size_t n = 0;
    for (const auto& result: results) {
        if (!result.found) {
            continue;
        }

        auto data_ptr = &data[n];
        strcpy(data_ptr->name, result.path.c_str());
        data_ptr->datatype = result.datatype;
        data_ptr->rank = result.rank;
        memcpy(data_ptr->dims, result.shape, 64 * sizeof(int));

        int count = 1;
        for (int i = 0; i < data_ptr->rank; ++i) {
            count *= result.shape[i];
        }

        size_t mem_size = count * sizeof_datatype(result.datatype);
        data_ptr->data = (unsigned char*)malloc(mem_size);
        addMalloc(plugin_interface->logmalloclist, (void*)data_ptr->data, mem_size, sizeof(unsigned char), "unsigned char");

        if (result.is_size) {
            memcpy((void*)data_ptr->data, &result.size, mem_size);
        } else if (result.using_buffer) {
            if (count > 1) {
                RAISE_PLUGIN_ERROR("too much data to read from result buffer");
            }
            memcpy((void*)data_ptr->data, result.buffer, mem_size);
        } else {
            memcpy((void*)data_ptr->data, result.data, mem_size);
        }

        ++n;
    }

    auto data_list = (DataList*)malloc(sizeof(DataList));
    addMalloc(plugin_interface->logmalloclist, (void*)data_list, 1, sizeof(DataList), "DataList");

    data_list->list = data;
    data_list->size = static_cast<int>(results.size());

    DATA_BLOCK* data_block = plugin_interface->data_block;
    initDataBlock(data_block);

    data_block->data_type = UDA_TYPE_COMPOUND;
    data_block->rank = 0;
    data_block->data_n = 1;
    data_block->dims = nullptr;
    data_block->data = (char*)data_list;

    data_block->opaque_type = UDA_OPAQUE_TYPE_STRUCTURES;
    data_block->opaque_count = 1;
    data_block->opaque_block = (void*)findUserDefinedType(plugin_interface->userdefinedtypelist, "DataList", 0);

    return 0;
}

/**
 * Open an IDS database entry, caching the pulse context handle.
 *
 * Arguments:
 *      backend (optional, string)  - IMAS backend used to read the IDS [ascii|mdsplus|hdf5] - defaults to mdsplus
 *      shot    (required, int)     - shot number
 *      run     (required, int)     - run number
 *      user    (required, string)  - user or path
 *      tokamak (required, string)  - tokamak name
 *      version (required, string)  - IMAS version
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
    const char* backend = "mdsplus";
    bool is_backend = FIND_STRING_VALUE(plugin_interface->request_data->nameValueList, backend);

    int shot;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, user);

    const char* tokamak;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, tokamak);

    const char* version;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, version);



    int ctx;
    al_status_t status = ual_begin_pulse_action(MDSPLUS_BACKEND, shot, run, user, tokamak, version, &ctx);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR("failed to begin pulse action");
    }

    status = ual_open_pulse(ctx, OPEN_PULSE, "");
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR("failed to open pulse");
    }

    initDataBlock(plugin_interface->data_block);

    Entry entry = {shot, run, user, tokamak, version};
    _open_entries[entry] = ctx;

    return setReturnDataIntScalar(plugin_interface->data_block, ctx, "pulse context");
}

/**
 * Closes the IMAS database entry corresponding to the given arguments. The entry must have been opened by calling the
 * open(...) or get(...) functions.
 *
 * Arguments:
 *      shot    (required, int)     - shot number
 *      run     (required, int)     - run number
 *      user    (required, string)  - user or path
 *      tokamak (required, string)  - tokamak name
 *      version (required, string)  - IMAS version
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
    int shot;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, shot);

    int run;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, run);

    const char* user;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, user);

    const char* tokamak;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, tokamak);

    const char* version;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, version);

    initDataBlock(plugin_interface->data_block);

    Entry entry = {shot, run, user, tokamak, version};
    if (_open_entries.count(entry) == 0) {
        RAISE_PLUGIN_ERROR("pulse is not currently open");
    }
    int ctx = _open_entries[entry];

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

    al_status_t status = ual_close_pulse(ctx, CLOSE_PULSE, "");
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }

    status = ual_end_action(ctx);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }
    _open_entries.erase(entry);

    return setReturnDataIntScalar(plugin_interface->data_block, -1, "pulse context");
}