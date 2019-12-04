#include "iter_md_plugin.h"

#include <memory>
#include <stack>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <libgen.h>
extern "C" {
#include <IDSDBHelper.h>
}

#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>

#include "configuration_mapping.h"

namespace iter {
namespace md {

class Plugin {
public:
    void init()
    {}

    void reset()
    {}

    int help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
    int read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

private:
    ConfigMapping config_mapping_ = {};
};

}
}

int itermdPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    try {
        static iter::md::Plugin plugin{};

        if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
            RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        }

        idam_plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, nullptr, 10);

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
        } else if (function == "read") {
            return plugin.read(idam_plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        }
    } catch (std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int iter::md::Plugin::help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    std::string path = __FILE__;
    std::string dir = dirname((char*)path.c_str());
    path = dir + "/help.md";
    std::ifstream ifs(path);
    std::stringstream ss;
    ss << ifs.rdbuf();

    std::string s = ss.str();
    const char* help = s.c_str();
    const char* desc = PLUGIN_NAME ": help = description of this plugin";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

/**
 * Plugin version
 * @param idam_plugin_interface
 * @return
 */
int iter::md::Plugin::version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, PLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int iter::md::Plugin::build_date(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int iter::md::Plugin::default_method(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int iter::md::Plugin::max_interface_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION,
                                  "Maximum Interface Version");
}

namespace {

int
setReturnDataInt64Array(DATA_BLOCK* data_block, int64_t* values, size_t rank, const size_t* shape, const char* description)
{
    initDataBlock(data_block);

    if (description != nullptr) {
        strncpy(data_block->data_desc, description, STRING_LENGTH);
        data_block->data_desc[STRING_LENGTH - 1] = '\0';
    }

    data_block->rank = (int)rank;
    data_block->dims = (DIMS*)malloc(rank * sizeof(DIMS));

    size_t len = 1;

    for (size_t i = 0; i < rank; ++i) {
        initDimBlock(&data_block->dims[i]);

        data_block->dims[i].data_type = UDA_TYPE_UNSIGNED_INT;
        data_block->dims[i].dim_n = (int)shape[i];
        data_block->dims[i].compressed = 1;
        data_block->dims[i].dim0 = 0.0;
        data_block->dims[i].diff = 1.0;
        data_block->dims[i].method = 0;

        len *= shape[i];
    }

    int* data = (int*)malloc(len * sizeof(int64_t));
    memcpy(data, values, len * sizeof(int64_t));

    data_block->data_type = UDA_TYPE_LONG64;
    data_block->data = (char*)data;
    data_block->data_n = (int)len;

    return 0;
}

}

namespace {

struct DatabaseWrapper {
    DatabaseWrapper() {
        status_ = initDBinfo(&dbi_);
        if (status_ != 0) {
            return;
        }
        status_ = connectPostgres(&dbi_);
        if (status_ != 0) {
            return;
        }
    }
    ~DatabaseWrapper() {
        if (status_ == 0) {
            db_exit(&dbi_);
        }
    }

    int status() const { return status_; }
    const dbinfo& dbi() const { return dbi_; }
private:
    int status_;
    dbinfo dbi_;
};

int read_size(const dbinfo& dbi, const char* config_name, const char* machine_version, const char* element, DataBlock* data_block)
{
    int size = getNumberOfEltperIDSStruct(dbi, config_name, machine_version, element);
    return setReturnDataIntScalar(data_block, size, "");
}

int read_value(const dbinfo& dbi, const char* config_name, const char* machine_version, const char* element, DataBlock* data_block)
{
    machDetails* md = getMachineDetailsPerMNameAndIDSPath(dbi, config_name, machine_version, element);
    if (md == nullptr) {
        RAISE_PLUGIN_ERROR("Error while trying to retrieve machine details from database");
    }

    if (std::string{ "FLT_0D" } == md->dtype) {
        setReturnDataDoubleScalar(data_block, md->valf, "");
    } else if (std::string{ "INT_0D" } == md->dtype) {
        setReturnDataIntScalar(data_block, md->vali, "");
    } else if (std::string{ "FLT_1D" } == md->dtype) {
        size_t shape[] = { (size_t)md->nb_elt };
        setReturnDataDoubleArray(data_block, md->valaf, 1, shape, "");
    } else if (std::string{ "INT_1D" } == md->dtype) {
        size_t shape[] = { (size_t)md->nb_elt };
        setReturnDataInt64Array(data_block, md->valai, 1, shape, "");
    } else if (std::string{ "STR_0D" } == md->dtype) {
        setReturnDataString(data_block, md->vals, "");
    }
}

char* insert_node_indices(const char* in_string, const int* indices, size_t n_indices)
{
    char* out_string = strdup(in_string);

    const char* p;
    size_t n = 0;

    while ((p = strchr(out_string, '#')) != nullptr) {
        assert(n < n_indices);
        size_t len = snprintf(nullptr, 0, "%d", indices[n]);
        char num_str[len + 1];
        snprintf(num_str, len + 1, "%d", indices[n]);
        ++n;

        char* pre = strndup(out_string, (int)(p - out_string));

        len = strlen(pre) + strlen(num_str) + strlen(p + 1) + 1;
        auto temp = (char*)malloc((len + 1) * sizeof(char));
        snprintf(temp, len, "%s%s%s", pre, num_str, p + 1);
        free(out_string);
        out_string = temp;

        free(pre);
    }

    return out_string;
}

} // anon namespace

int iter::md::Plugin::read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    initDataBlock(data_block);

    data_block->rank = 0;
    data_block->dims = nullptr;

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* element = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);

    int shot = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

    int run = 0;
    FIND_INT_VALUE(request_block->nameValueList, run);

    int* indices = nullptr;
    size_t nindices = 0;
    FIND_REQUIRED_INT_ARRAY(request_block->nameValueList, indices);

    if (nindices == 1 && indices[0] == -1) {
        nindices = 0;
        free(indices);
        indices = nullptr;
    }

    int dtype = 0;
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, dtype);

    const char* IDS_version = nullptr;
    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, IDS_version);

    const char* experiment = nullptr;
    FIND_STRING_VALUE(request_block->nameValueList, experiment);

    // keep old way of passing experiment until IMAS plugin has been updated
    if (experiment == nullptr) {
        experiment = request_block->archive;
    }

    int ret = 0;

    const DatabaseWrapper db;
    if (db.status() != 0) {
        RAISE_PLUGIN_ERROR("Error while trying to connecting to the database");
    }

    std::string config_name = config_mapping_.config_name(shot);
    if (config_name.empty()) {
        std::string msg = std::string("no configuration found for shot ") + std::to_string(shot);
        RAISE_PLUGIN_ERROR(msg.c_str());
    }

    std::string machine_version = config_mapping_.machine_version(shot);
    if (machine_version.empty()) {
        std::string msg = std::string("no machine version found for shot ") + std::to_string(shot);
        RAISE_PLUGIN_ERROR(msg.c_str());
    }

    initDataBlock(idam_plugin_interface->data_block);

    char* path = insert_node_indices(element, indices, nindices);

    if (boost::ends_with(path, "/Shape_of")) {
        path[strlen(path) - 9] = '\0';
        return read_size(db.dbi(), config_name.c_str(), machine_version.c_str(), path, idam_plugin_interface->data_block);
    } else {
        return read_value(db.dbi(), config_name.c_str(), machine_version.c_str(), path, idam_plugin_interface->data_block);
    }
}
