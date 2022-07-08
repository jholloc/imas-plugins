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

    COMPOUNDFIELD name_field;
    initCompoundField(&name_field);
    strcpy(name_field.name, "name");
    name_field.atomictype = UDA_TYPE_STRING;
    strcpy(name_field.type, "STRING");
    strcpy(name_field.desc, "data name");
    name_field.pointer = 1;
    name_field.count = 1;
    name_field.rank = 0;
    name_field.shape = nullptr;
    name_field.size = name_field.count * sizeof(char*);
    name_field.offset = offsetof(Data, name);
    name_field.offpad = padding(offsetof(Data, name), name_field.type);
    name_field.alignment = getalignmentof(name_field.type);
    addCompoundField(&data_usertype, name_field);

    ::addStructureField(&data_usertype, "data", "data pointer", UDA_TYPE_UNSIGNED_CHAR, true, 0, nullptr,
                        offsetof(Data, data));
    ::addStructureField(&data_usertype, "rank", "data rank", UDA_TYPE_INT, false, 0, nullptr, offsetof(Data, rank));
    int shape[] = {64};
    ::addStructureField(&data_usertype, "dims", "data dimensions", UDA_TYPE_INT, false, 1, shape, offsetof(Data, dims));
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

    COMPOUNDFIELD size_field;
    initCompoundField(&size_field);

    int offset = (int)offsetof(DataList, size);
    defineField(&size_field, "size", "number of data", &offset, SCALARINT);
    addCompoundField(&data_list_usertype, size_field);

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

struct ArraystructCache
{
    std::string node;
    int ctx;
    int size;
};

struct IDSData
{
    std::string path;
    bool found = false;
    void* data = nullptr;
    char buffer[8];
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
        define_usertypes(plugin_interface->userdefinedtypelist);
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

private:
    bool _init = false;
    std::unordered_map<Entry, int> _open_entries = {};
    ArraystructCache _arraystruct_cache = {"", -1, 0};

    std::vector<IDSData>
    read_data(int ctx, const std::string& timebase, std::deque<std::string>& tokens, int datatype, int rank,
              const std::string& ids);

    void read_data_r(int ctx, const std::string& timebase, std::deque<std::string>& tokens, int datatype, int rank,
                     std::vector<IDSData>& return_data, const std::string& path);
};

}
}
}

int imasPlugin(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    try {
        static uda::plugins::imas::Plugin plugin{};

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

        if (function == "init" || function == "initialise") {
            return plugin.init(plugin_interface);
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
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
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

void
uda::plugins::imas::Plugin::read_data_r(int ctx, const std::string& timebase, std::deque<std::string>& tokens,
                                        int datatype, int rank, std::vector<IDSData>& return_data, const std::string& path)
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
            data.data = &data.buffer;
        }

        al_status_t status = ual_read_data(ctx, node.c_str(), timebase.c_str(), &data.data, datatype, rank, data.shape);
        if (status.code != 0) {
            throw std::runtime_error{status.message};
        }

        data.found = true;
        return_data.push_back(data);
    } else if (!tokens.empty()) {
        // handle arraystruct case

        assert(boost::ends_with(tokens.front(), "[:]"));

        auto head = tokens.front();
        tokens.pop_front();

        node = head.substr(0, head.size() - 3);

        int arr_ctx;
        int size;

        if (_arraystruct_cache.node == node) {
            arr_ctx = _arraystruct_cache.ctx;
            size = _arraystruct_cache.size;
        } else {
            if (!_arraystruct_cache.node.empty()) {
                al_status_t status = ual_end_action(_arraystruct_cache.ctx);
                _arraystruct_cache = {"", -1, 0,};
                if (status.code != 0) {
                    throw std::runtime_error{status.message};
                }
            }

            al_status_t status = ual_begin_arraystruct_action(ctx, node.c_str(), timebase.c_str(), &size, &arr_ctx);
            if (status.code != 0) {
                throw std::runtime_error{status.message};
            }
        }

        int i = 0;
        while (i < size) {
            std::string new_path = path;
            new_path.append("/").append(node).append("[").append(std::to_string(i)).append("]");
            std::deque<std::string> copy = tokens;
            read_data_r(arr_ctx, timebase, copy, datatype, rank, return_data, new_path);
            ual_iterate_over_arraystruct(arr_ctx, 1);
            ++i;
        }
    }
}

std::vector<uda::plugins::imas::IDSData>
uda::plugins::imas::Plugin::read_data(int ctx, const std::string& timebase, std::deque<std::string>& tokens,
                                      int datatype, int rank, const std::string& ids)
{
    std::vector<IDSData> return_data;
    read_data_r(ctx, timebase, tokens, datatype, rank, return_data, ids);

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
 * Returns the IMAS data for the IDS leaf node.
 * @return 0 if successful, !0 on error
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

    const char* timebase = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, timebase);

    const char* datatype = nullptr;
    FIND_REQUIRED_STRING_VALUE(plugin_interface->request_data->nameValueList, datatype);

    int rank = -1;
    FIND_REQUIRED_INT_VALUE(plugin_interface->request_data->nameValueList, rank);

    int ctxId = -1;
    FIND_INT_VALUE(plugin_interface->request_data->nameValueList, ctxId);

    Entry entry = {shot, run, user, tokamak, version};
    if (!_open_entries.count(entry)) {
        int rc = open(plugin_interface);
        if (rc != 0) {
            return rc;
        }
    }
    ctxId = _open_entries[entry];

    std::deque<std::string> tokens;
    boost::split(tokens, path, boost::is_any_of("/"), boost::token_compress_on);

    auto ids = tokens.front();
    tokens.pop_front();

    std::string s_access = access;

    int op_ctx;
    al_status_t status = ual_begin_global_action(ctxId, ids.c_str(), convert_access_mode(access), &op_ctx);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }

    std::vector<IDSData> results = {};
    try {
        results = read_data(op_ctx, timebase, tokens, convert_datatype(datatype), rank, ids);
        if (results.empty()) {
            initDataBlock(plugin_interface->data_block);
            return 0;
        }
    } catch (std::runtime_error& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }

    status = ual_end_action(ctxId);
    if (status.code != 0) {
        RAISE_PLUGIN_ERROR(status.message);
    }

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
        data_ptr->data = (unsigned char*)malloc(count * sizeof_datatype(result.datatype));
        addMalloc(plugin_interface->logmalloclist, (void*)data_ptr->data, count, sizeof_datatype(result.datatype),
                  datatype);

        memcpy((void*)data_ptr->data, result.data, count * sizeof_datatype(result.datatype));

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

int uda::plugins::imas::Plugin::open(IDAM_PLUGIN_INTERFACE* plugin_interface)
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