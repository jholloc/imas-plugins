#include "imas_partial_plugin.h"

#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <map>
#include <memory>
#include <deque>
#include <boost/algorithm/string.hpp>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <clientserver/copyStructs.h>
#include <clientserver/makeRequestBlock.h>
#include <plugins/udaPlugin.h>
#include <structures/struct.h>
#include <structures/accessors.h>

#include "access_functions.h"
#include "pugixml.hpp"
#include "is_integer.h"

namespace {

class IMASPartialPlugin {
public:
    IMASPartialPlugin() = default;

    void init();
    void reset();

    int help(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int open(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int get(IDAM_PLUGIN_INTERFACE* plugin_interface);

private:
    pugi::xml_document doc_ = {};
    LLenv env_ = {};
};

#ifndef addStructureField

const char* udaNameType(UDA_TYPE type)
{
    switch (type) {
        case UDA_TYPE_CHAR:
            return "char";
        case UDA_TYPE_SHORT:
            return "short";
        case UDA_TYPE_INT:
            return "int";
        case UDA_TYPE_LONG:
            return "int";
        case UDA_TYPE_LONG64:
            return "long long";
        case UDA_TYPE_FLOAT:
            return "float";
        case UDA_TYPE_DOUBLE:
            return "double";
        case UDA_TYPE_UNSIGNED_CHAR:
            return "unsigned char";
        case UDA_TYPE_UNSIGNED_SHORT:
            return "unsigned short";
        case UDA_TYPE_UNSIGNED_INT:
            return "unsigned int";
        case UDA_TYPE_UNSIGNED_LONG:
            return "unsigned int";
        case UDA_TYPE_UNSIGNED_LONG64:
            return "unsigned long long";
        case UDA_TYPE_STRING:
            return "char";
        default:
            return "unknown";
    }
}

size_t getPtrSizeOf(UDA_TYPE data_type)
{
    switch (data_type) {
        case UDA_TYPE_FLOAT:
            return sizeof(float*);
        case UDA_TYPE_DOUBLE:
            return sizeof(double*);
        case UDA_TYPE_CHAR:
            return sizeof(char*);
        case UDA_TYPE_SHORT:
            return sizeof(short*);
        case UDA_TYPE_INT:
            return sizeof(int*);
        case UDA_TYPE_LONG:
            return sizeof(long*);
        case UDA_TYPE_LONG64:
            return sizeof(long long*);
        case UDA_TYPE_UNSIGNED_CHAR:
            return sizeof(unsigned char*);
        case UDA_TYPE_UNSIGNED_SHORT:
            return sizeof(unsigned short*);
        case UDA_TYPE_UNSIGNED_INT:
            return sizeof(unsigned int*);
        case UDA_TYPE_UNSIGNED_LONG:
            return sizeof(unsigned long*);
        case UDA_TYPE_UNSIGNED_LONG64:
            return sizeof(unsigned long long*);
        case UDA_TYPE_STRING:
            return sizeof(char*);
        case UDA_TYPE_COMPLEX:
            return sizeof(COMPLEX*);
        case UDA_TYPE_DCOMPLEX:
            return sizeof(DCOMPLEX*);
        default:
            return 0;
    }
}

void addStructureField(USERDEFINEDTYPE* user_type, const char* name, const char* desc, UDA_TYPE data_type, bool is_pointer, int rank, int* shape, size_t offset)
{
    COMPOUNDFIELD field;
    initCompoundField(&field);

    strcpy(field.name, name);
    field.atomictype = data_type;
    if (data_type == UDA_TYPE_STRING) {
        strcpy(field.type, "STRING");
    } else {
        strcpy(field.type, ::udaNameType(data_type));
    }
    if (is_pointer) {
        strcpy(&field.type[strlen(field.type)], " *");
    }
    strcpy(field.desc, desc);
    field.pointer = is_pointer;
    field.rank = rank;
    field.count = 1;
    if (shape != nullptr) {
        field.shape = (int*)malloc(field.rank * sizeof(int));
        int i;
        for (i = 0; i < rank; ++i) {
            field.shape[i] = shape[i];
            field.count *= shape[i];
        }
    }
    field.size = is_pointer ? (int)::getPtrSizeOf(data_type) : (field.count * (int)getSizeOf(data_type));
    field.offset = (int)offset;
    field.offpad = (int)padding(offset, field.type);
    field.alignment = getalignmentof(field.type);

    addCompoundField(user_type, field);
}

#endif

} // anon namespace

int imasPartial(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static IMASPartialPlugin plugin;

        if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
            RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        }

        idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

        REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

        if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
            plugin.reset();
            return 0;
        }

        plugin.init();

        if (STR_IEQUALS(request_block->function, "init") || STR_IEQUALS(request_block->function, "initialise")) {
            return 0;
        }

        if (STR_IEQUALS(request_block->function, "help")) {
            return plugin.help(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "version")) {
            return plugin.version(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "builddate")) {
            return plugin.build_date(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "defaultmethod")) {
            return plugin.default_method(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "maxinterfaceversion")) {
            return plugin.max_interface_version(idam_plugin_interface);
        } else if (STR_IEQUALS(request_block->function, "get")) {
            return plugin.get(idam_plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

void IMASPartialPlugin::init()
{
    const char* env = getenv("UDA_IMAS_PARTIAL_IDSDEF_PATH");
    if (env == nullptr) {
        throw std::runtime_error("UDA_IMAS_PARTIAL_IDSDEF_PATH environmental variable not set");
    }
    std::string dd_path = env;

    pugi::xml_parse_result result = doc_.load_file(dd_path.c_str());
    if (!result) {
        throw std::runtime_error(result.description());
    }
}

void IMASPartialPlugin::reset()
{

}

int IMASPartialPlugin::help(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    const char* help = "\ntemplatePlugin: Add Functions Names, Syntax, and Descriptions\n\n";
    const char* desc = "templatePlugin: help = description of this plugin";

    return setReturnDataString(plugin_interface->data_block, help, desc);
}

int IMASPartialPlugin::version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

int IMASPartialPlugin::build_date(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, __DATE__, "Plugin build date");
}

int IMASPartialPlugin::default_method(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataString(plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

int IMASPartialPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    return setReturnDataIntScalar(plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}

namespace {

std::string get_type(const std::string& data_type)
{
    std::vector<std::string> tokens;
    boost::split(tokens, data_type, boost::is_any_of("_"), boost::token_compress_on);
    if (tokens[0] == "INT" || tokens[0] == "int") {
        return "integer";
    } else if (tokens[0] == "FLT" || tokens[0] == "flt") {
        return "float";
    } else if (tokens[0] == "STR") {
        return "string";
    }
    throw std::runtime_error("unknown type " + tokens[0]);
}

int get_rank(const std::string& data_type)
{
    std::vector<std::string> tokens;
    boost::split(tokens, data_type, boost::is_any_of("_"), boost::token_compress_on);
    if (tokens.back() == "type") {
        return 0;
    }
    return std::stoi(tokens.back());
}

void get_requests(LLenv& env, std::vector<std::string>& requests, std::map<std::string, int>& sizes,
        std::string ids_path, const pugi::xml_node& node)
{
    std::string dtype = node.attribute("data_type").value();

    if (dtype == "struct_array") {
        int size = imas_partial::read_size_from_backend(env, ids_path);
        sizes[ids_path] = size;

        std::vector<std::string> tokens;
        boost::split(tokens, ids_path, boost::is_any_of("/"), boost::token_compress_on);

        if (imas_partial::is_integer(tokens.back())) {
            long val = strtol(tokens.back().c_str(), nullptr, 10);
            if (val > size) {
                throw std::range_error("out of range value given in ids_path");
            }
            for (const auto& child : node.children("field")) {
                get_requests(env, requests, sizes, ids_path + "/" + child.attribute("name").value(), child);
            }
        } else {
            for (int i = 1; i < size + 1; ++i) {
                std::string path = ids_path + "/" + std::to_string(i);
                for (const auto& child : node.children("field")) {
                    get_requests(env, requests, sizes, path + "/" + child.attribute("name").value(), child);
                }
            }
        }
    } else if (!dtype.empty() && dtype != "structure") {
        requests.push_back(ids_path);

        for (const auto& child : node.children("field")) {
            get_requests(env, requests, sizes, ids_path + "/" + child.attribute("name").value(), child);
        }
    } else {
        for (const auto& child : node.children("field")) {
            get_requests(env, requests, sizes, ids_path + "/" + child.attribute("name").value(), child);
        }
    }
}

void expand_request(std::vector<std::string>& requests, size_t depth, const std::string& request, const std::deque<int>& sizes)
{
    size_t pos = 0;
    if ((pos = request.find('#')) == std::string::npos) {
        requests.push_back(request);
        return;
    }
    if (depth > sizes.size()) {
        throw std::runtime_error("size not found");
    }
    for (int i = 0; i < sizes[depth]; ++i) {
        auto sub_request = request;
        sub_request.replace(pos, 1, std::to_string(i+1));
        expand_request(requests, depth + 1, sub_request, sizes);
    }
}

pugi::xml_node get_sibling_by_index(pugi::xml_node node, long index)
{
    for (long i = 1; i < index; ++i) {
        node = node.next_sibling();
    }
    return node;
}

int strtoi(const char* str, char** endptr, int base)
{
    long num = strtol(str, endptr, base);
    if (num > std::numeric_limits<int>::max()) {
        throw std::out_of_range{ std::to_string(num) + " out of range for reading as an integer" };
    }
    return static_cast<int>(num);
}

int imas_to_uda_type(int imas_type)
{
    switch (imas_type) {
        case CHAR_DATA: return UDA_TYPE_CHAR;
        case INTEGER_DATA: return UDA_TYPE_INT;
        case DOUBLE_DATA: return UDA_TYPE_DOUBLE;
        case COMPLEX_DATA: return UDA_TYPE_COMPLEX;
        default: return UDA_TYPE_UNKNOWN;
    }
}

int imas_type_size(int imas_type)
{
    switch (imas_type) {
        case CHAR_DATA: return sizeof(char);
        case INTEGER_DATA: return sizeof(int);
        case DOUBLE_DATA: return sizeof(double);
        case COMPLEX_DATA: return sizeof(Complex);
        default:
            throw std::runtime_error("unknown imas type");
    }
}

LLenv open_pulse(IDAM_PLUGIN_INTERFACE* plugin_interface, int shot, int run, const char* user, const char* tokamak, const char* version)
{
    int ctxId = Lowlevel::beginPulseAction(ualconst::mdsplus_backend, shot, run, user, tokamak, version);
    LLenv env = Lowlevel::getLLenv(ctxId);

    int mode = ualconst::open_pulse;
    const char* options = "";

    auto pulseCtx = dynamic_cast<PulseContext*>(env.context);

    env.backend->openPulse(pulseCtx, mode, options);

    return env;
}

} // anon namespace

int IMASPartialPlugin::get(IDAM_PLUGIN_INTERFACE* plugin_interface)
{
    REQUEST_BLOCK* request_block = plugin_interface->request_block;

    int shot = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

    const char* user = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, user);

    const char* tokamak = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, tokamak);

    const char* version = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, version);

    const char* path = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, path);

    if (env_.backend == nullptr) {
        env_ = open_pulse(plugin_interface, shot, run, user, tokamak, version);
    }

    if (path == nullptr || path[0] == '\0') {
        RAISE_PLUGIN_ERROR("invalid path provided");
    }

    if (path[0] == '/') {
        path = &path[1];
    }

    std::deque<std::string> tokens;
    boost::split(tokens, path, boost::is_any_of("/"), boost::token_compress_on);

    auto nodes = doc_.child("IDSs");
    std::string token;

    std::vector<std::string> ids_paths;
    std::vector<std::string> size_requests;

    ids_paths.push_back("");
    std::string delim;

    while (!tokens.empty()) {
        token = tokens.front();

        if (imas_partial::is_integer(token)) {
            for (auto& ids_path : ids_paths) {
                if (tokens.size() != 1) {
                    size_requests.push_back(ids_path);
                }
                ids_path += delim + token;
            }
        } else if (imas_partial::is_range(token)) {
            std::vector<std::string> new_paths;
            for (const auto& ids_path : ids_paths) {
                if (tokens.size() != 1) {
                    size_requests.push_back(ids_path);
                }
                auto range = imas_partial::parse_range(token);
                for (long i = range.begin; i < range.end + 1; i += range.stride) {
                    std::string new_path = ids_path + delim + std::to_string(i);
                    new_paths.push_back(new_path);
                }
            }
            ids_paths = new_paths;
        } else {
            nodes = nodes.find_child_by_attribute("name", token.c_str());
            for (auto& ids_path : ids_paths) {
                ids_path += delim + token;
            }
        }

        delim = "/";
        tokens.pop_front();
    }

    std::map<std::string, int> sizes;

    for (const auto& request : size_requests) {
        int size = imas_partial::read_size_from_backend(env_, request);
        sizes[request] = size;
    }

    std::vector<std::string> requests;

    for (const auto& ids_path: ids_paths) {
        get_requests(env_, requests, sizes, ids_path, nodes);
    }

    std::map<std::string, imas_partial::MDSData> results;

    for (const auto& pair : sizes) {
        std::cout << pair.first << " = " << pair.second << std::endl;
        imas_partial::MDSData data{};
        data.datatype = INTEGER_DATA;
        data.data = malloc(sizeof(int));
        *((int*)data.data) = pair.second;
        results[pair.first] = data;
    }

    for (const auto& request : requests) {
        std::cout << request << std::endl;

        imas_partial::MDSData mds_data = imas_partial::read_data_from_backend(env_, request);
        results[request] = mds_data;
    }

    closeIdamError();

    USERDEFINEDTYPE data_usertype;
    initUserDefinedType(&data_usertype);

    strcpy(data_usertype.name, "Data");
    strcpy(data_usertype.source, "IMAS_PARTIAL");
    data_usertype.ref_id = 0;
    data_usertype.imagecount = 0; // No Structure Image data
    data_usertype.image = nullptr;
    data_usertype.size = sizeof(Data); // Structure size
    data_usertype.idamclass = UDA_TYPE_COMPOUND;

//    addStructureField(&data_usertype, "name", "data name", UDA_TYPE_STRING, true, 0, nullptr, offsetof(Data, name));

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

    ::addStructureField(&data_usertype, "data", "data pointer", UDA_TYPE_UNSIGNED_CHAR, true, 0, nullptr, offsetof(Data, data));
    ::addStructureField(&data_usertype, "rank", "data rank", UDA_TYPE_INT, false, 0, nullptr, offsetof(Data, rank));
    int shape[] = { 64 };
    ::addStructureField(&data_usertype, "dims", "data dimensions", UDA_TYPE_INT, false, 1, shape, offsetof(Data, dims));
    ::addStructureField(&data_usertype, "datatype", "data type", UDA_TYPE_INT, false, 0, nullptr, offsetof(Data, datatype));

    addUserDefinedType(plugin_interface->userdefinedtypelist, data_usertype);

    USERDEFINEDTYPE data_list_usertype;
    initUserDefinedType(&data_list_usertype);

    strcpy(data_list_usertype.name, "DataList");
    strcpy(data_list_usertype.source, "IMAS_PARTIAL");
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

    addUserDefinedType(plugin_interface->userdefinedtypelist, data_list_usertype);

    Data* data = (Data*)calloc(results.size(), sizeof(Data));
    addMalloc(plugin_interface->logmalloclist, (void*)data, (int)results.size(), sizeof(Data), "Data");

    size_t i = 0;
    for (const auto& pair : results) {
        Data* dp = &data[i];
        memset(dp->dims, '\0', 64 * sizeof(int));

        dp->name = strdup(pair.first.c_str());
        addMalloc(plugin_interface->logmalloclist, (void*)dp->name, 1, strlen(dp->name) + 1, "char");

        size_t size = 1;
        for (int j = 0; j < pair.second.rank; ++j) {
            size *= pair.second.dims[j];
            dp->dims[j] = pair.second.dims[j];
        }
        dp->rank = pair.second.rank;

        if (pair.second.data != nullptr) {
            size_t sz = size * imas_type_size(pair.second.datatype);
            dp->data = (unsigned char*)malloc(sz);
            memcpy((void*)dp->data, pair.second.data, sz);
            addMalloc(plugin_interface->logmalloclist, (void*)dp->data, (int)sz, 1, "unsigned char");
            dp->datatype = pair.second.datatype;
        } else {
            dp->data = nullptr;
            dp->datatype = UDA_TYPE_VOID;
        }

        ++i;
    }

    auto data_list = new DataList{ data, (int)results.size() };
    addMalloc(plugin_interface->logmalloclist, (void*)data_list, 1, sizeof(DataList), "DataList");

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
