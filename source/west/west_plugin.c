#include "west_plugin.h"

#include <assert.h>
#include <string.h>

#include <clientserver/errorLog.h>
#include <logging/logging.h>
#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <plugins/udaPlugin.h>

#include "west_xml.h"
#include "west_dynamic_data.h"

enum MAPPING_TYPE {
    NONE, STATIC, DYNAMIC
};

static int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static char* getMappingValue(const char* mappingFileName, const char* IDSRequest, int* IDSRequestType);

int westPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    int err = 0;

    static short init = 0;

    // ----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    unsigned short housekeeping;

    if (idam_plugin_interface->interfaceVersion >
        THISPLUGIN_MAX_INTERFACE_VERSION) {
        err = 999;
        //UDA_LOG(UDA_LOG_ERROR, "Plugin Interface Version Unknown to this plugin: Unable to execute the request!\n");
        addIdamError(CODEERRORTYPE, __func__,
                     err, "Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        return err;
    }

    idam_plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, NULL, 10);

    REQUEST_DATA* request_data = idam_plugin_interface->request_data;

    housekeeping = idam_plugin_interface->housekeeping;

    if (housekeeping || STR_IEQUALS(request_data->function, "reset")) {

        if (!init) {
            return 0;
        }    // Not previously initialised: Nothing to
        // do!

        // Free Heap & reset counters

        init = 0;

        return 0;
    }
    // ----------------------------------------------------------------------------------------
    // Initialise

    if (!init || STR_IEQUALS(request_data->function, "init")
        || STR_IEQUALS(request_data->function, "initialise")) {

        init = 1;
        if (STR_IEQUALS(request_data->function, "init")
            || STR_IEQUALS(request_data->function, "initialise")) {
            return 0;
        }
    }
    // ----------------------------------------------------------------------------------------
    // Plugin Functions
    // ----------------------------------------------------------------------------------------

    // ----------------------------------------------------------------------------------------
    // Standard methods: version, builddate, defaultmethod,
    // maxinterfaceversion

    if (STR_IEQUALS(request_data->function, "help")) {
        err = do_help(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "version")) {
        err = do_version(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "builddate")) {
        err = do_builddate(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "defaultmethod")) {
        err = do_defaultmethod(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "maxinterfaceversion")) {
        err = do_maxinterfaceversion(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "read")) {
        err = do_read(idam_plugin_interface);
    } else if (STR_IEQUALS(request_data->function, "close")) {
        err = 0;
    }else {
        // ======================================================================================
        // Error ...
        err = 999;
        UDA_LOG(UDA_LOG_DEBUG, "unknown requested function is: %s\n", request_data->function);
        addIdamError(CODEERRORTYPE, __func__, err, "WEST:ERROR: unknown function requested!");
    }

    // --------------------------------------------------------------------------------------
    // Housekeeping

    return err;
}

// Help: A Description of library functionality
int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    char* help = PLUGIN_NAME ": this plugin maps WEST data to IDSs\n\n";
    const char* desc = PLUGIN_NAME ": help = plugin used for mapping WEST experimental data to IDS";
}

int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, PLUGIN_VERSION, NULL);
}

// Plugin Build Date
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, NULL);
}

// Plugin Default Method
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, NULL);
}

// Plugin Maximum Interface Version
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, NULL);
}

// ----------------------------------------------------------------------------------------
// Add functionality here ....
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    UDA_LOG(UDA_LOG_DEBUG, "Calling do_read function from WEST plugin\n");

    int err = 0;

    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    initDataBlock(data_block);

    data_block->rank = 1;
    data_block->dims = (DIMS*)malloc(data_block->rank * sizeof(DIMS));

    int i;
    for (i = 0; i < data_block->rank; i++) {
        initDimBlock(&data_block->dims[i]);
    }

    REQUEST_DATA* request_data = idam_plugin_interface->request_data;

    const char* element;    // will contain the UDA mapping got from the UDA request
    int shot;
    int* indices;
    size_t nindices;

    FIND_REQUIRED_STRING_VALUE(request_data->nameValueList, element);
    FIND_REQUIRED_INT_VALUE(request_data->nameValueList, shot);
    FIND_REQUIRED_INT_ARRAY(request_data->nameValueList, indices);

    UDA_LOG(UDA_LOG_INFO, "Calling %s for shot: %d\n", element, shot);

    const char* UDA_MappingKey = element;

    char* mappingFileName = getenv("UDA_WEST_MAPPING_FILE");

    UDA_LOG(UDA_LOG_DEBUG, "UDA mapping file: %s\n", mappingFileName);
    UDA_LOG(UDA_LOG_DEBUG, "UDA mapping key: %s\n", UDA_MappingKey);

    //Get the mapping function from the value found in the UDA mapping file for the given UDA_MappingKey
    //Get also the IDS type ('static' or 'dynamic')
    int IDS_DataType;
    const char* mapfun = getMappingValue(mappingFileName, UDA_MappingKey, &IDS_DataType);

    //The path requested has not been found
    if (mapfun == NULL) {
        //UDA_LOG(UDA_LOG_ERROR, "The requested mapping function has not been found. Check the UDA mapping file.\n");
        //fprintf(stderr, "The requested mapping function has not been found. Check the UDA mapping file.");
        int err = 901;
        addIdamError(CODEERRORTYPE, "The requested mapping function has not been found. Check the UDA mapping file.", err, "");
        return -1;
    }

    //STATIC DATA CASE
    if (IDS_DataType == STATIC) {

        UDA_LOG(UDA_LOG_DEBUG, "Fetching static data from WEST plugin\n");

        // Executing TSLib for getting static data
        int status = GetStaticData(shot, mapfun, data_block, indices);
        if (status != 0) {
            return status;
        }

        int data_type = data_block->data_type;

        UDA_LOG(UDA_LOG_DEBUG, "Requested data type: %d\n", data_type);

        if (data_type != UDA_TYPE_STRING &&
            data_type != UDA_TYPE_DOUBLE &&
            data_type != UDA_TYPE_FLOAT &&
            data_type != UDA_TYPE_LONG &&
            data_type != UDA_TYPE_INT &&
            data_type != UDA_TYPE_SHORT) {
            err = 999;
            addIdamError(CODEERRORTYPE, __func__, err, "WEST:ERROR: unsupported data type");
        }

        return 0;

    } else if (IDS_DataType == DYNAMIC) {

        // DYNAMIC DATA CASE
        UDA_LOG(UDA_LOG_DEBUG, "Fetching dynamic data from WEST plugin\n");
        int status = GetDynamicData(shot, mapfun, data_block, indices);
        return status;
    }

    return 0;
}



//Get from the UDA mapping file the IDS XPath for the given key and the data type ('static' or 'dynamic')
//Example : <mapping key="antennas/ec/Shape_of" value="//antennas/ec/@dim" type="static"/>
// where the key is 'antennas/ec/Shape_of' and the IDS XPath is '//antennas/ec/@dim', the type is 'static'

char* getMappingValue(const char* mappingFileName, const char* UDA_MappingKey,
                      int* IDS_DataType)
{
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;

    assert(mappingFileName);
    assert(UDA_MappingKey);

    /*
     * Load XML document
     */
    doc = xmlParseFile(mappingFileName);
    if (doc == NULL) {
        //UDA_LOG(UDA_LOG_ERROR, "WEST:ERROR: unable to parse UDA mapping file %s\n", mappingFileName);
        int err = 901;
        addIdamError(CODEERRORTYPE, "WEST:ERROR: unable to parse UDA mapping file %s !", err, mappingFileName);
        return NULL;
    }

    /*
     * Create xpath evaluation context
     */
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        //UDA_LOG(UDA_LOG_ERROR, "WEST:ERROR: unable to create new XPath context\n");
        int err = 901;
        addIdamError(CODEERRORTYPE, "WEST:ERROR: unable to create new XPath context", err, "");
        xmlFreeDoc(doc);
        return NULL;
    }
    // Creating the Xpath request
    UDA_LOG(UDA_LOG_DEBUG, "Creating the Xpath request\n");
    int len = strlen(UDA_MappingKey) + 26;
    xmlChar* xPathExpr = malloc(len + sizeof(xmlChar));
    const char* c = "//mapping[@key='%s']/@value";
    xmlStrPrintf(xPathExpr, len, c, UDA_MappingKey);

    /*
     * Evaluate xpath expression for the type
     */
    xpathObj = xmlXPathEvalExpression(xPathExpr, xpathCtx);
    if (xpathObj == NULL) {
        //UDA_LOG(UDA_LOG_ERROR, "WEST:ERROR: unable to evaluate xpath expression %s\n", c);
        int err = 901;
        addIdamError(CODEERRORTYPE, "WEST:ERROR: unable to evaluate xpath expression %s\n", err, c);
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    int size = (nodes) ? nodes->nodeNr : 0;
    char* value = NULL;

    xmlNodePtr cur;
    int err = 0;

    if (size != 0) {
        UDA_LOG(UDA_LOG_DEBUG, "size different of 0\n");
        cur = nodes->nodeTab[0];
        cur = cur->children;
        value = strdup((char*)cur->content);
    } else {
        UDA_LOG(UDA_LOG_DEBUG, "Error : size equals 0\n");
        err = 902;
        addIdamError(CODEERRORTYPE, __func__, err, "no result on XPath request");
    }
    const char* key_type = "//mapping[@key='%s']/@type";
    xmlStrPrintf(xPathExpr, len, key_type,
                 UDA_MappingKey);

    /*
     * Evaluate xpath expression for the type
     */
    xpathObj = xmlXPathEvalExpression(xPathExpr, xpathCtx);
    if (xpathObj == NULL) {
        //UDA_LOG(UDA_LOG_ERROR,
        //         "WEST:ERROR: unable to evaluate xpath expression for getting the type (static or dynamic): %s\n", key_type);
        err = 901;
        addIdamError(CODEERRORTYPE, "WEST:ERROR: unable to evaluate following xpath expression for getting the type (static or dynamic): %s\n", err, key_type);
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return NULL;
    }

    nodes = xpathObj->nodesetval;
    size = (nodes) ? nodes->nodeNr : 0;
    char* typeStr = NULL;

    err = 0;

    if (size != 0) {
        cur = nodes->nodeTab[0];
        cur = cur->children;
        typeStr = strdup((char*)cur->content);
    } else {
        err = 902;
        //UDA_LOG(UDA_LOG_ERROR, "no result on XPath request\n");
        addIdamError(CODEERRORTYPE, __func__, err, "no result on XPath request");
    }

    UDA_LOG(UDA_LOG_DEBUG, "Setting IDS_DataType\n");

    if (typeStr == NULL) {
        *IDS_DataType = NONE;
    } else if (STR_IEQUALS(typeStr, "dynamic")) {
        *IDS_DataType = DYNAMIC;
    } else {
        *IDS_DataType = STATIC;
    }

    /*
     * Cleanup
     */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);

    return value;
}

