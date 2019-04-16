#include "hl2a_plugin.h"

#include <assert.h>
#include <string.h>
#include <libxml/xpath.h>

#include <clientserver/errorLog.h>
#include <logging/logging.h>
#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <plugins/udaPlugin.h>

#include "hl2a_mapped_data.h"

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

int hl2aPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    int err = 0;

    static short init = 0;

    // ----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    unsigned short housekeeping;

    if (idam_plugin_interface->interfaceVersion >
        THISPLUGIN_MAX_INTERFACE_VERSION) {
        err = 999;
        UDA_LOG(UDA_LOG_ERROR, "Plugin Interface Version Unknown to this plugin: Unable to execute the request!\n");
        addIdamError(CODEERRORTYPE, __func__,
                     err, "Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
        return err;
    }

    idam_plugin_interface->pluginVersion = strtol(PLUGIN_VERSION, NULL, 10);

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    housekeeping = idam_plugin_interface->housekeeping;

    if (housekeeping || STR_IEQUALS(request_block->function, "reset")) {

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

    if (!init || STR_IEQUALS(request_block->function, "init")
        || STR_IEQUALS(request_block->function, "initialise")) {

        init = 1;
        if (STR_IEQUALS(request_block->function, "init")
            || STR_IEQUALS(request_block->function, "initialise")) {
            return 0;
        }
    }
    // ----------------------------------------------------------------------------------------
    // Plugin Functions
    // ----------------------------------------------------------------------------------------

    // ----------------------------------------------------------------------------------------
    // Standard methods: version, builddate, defaultmethod,
    // maxinterfaceversion

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
        err = do_read(idam_plugin_interface);
    } else {
        // ======================================================================================
        // Error ...
        err = 999;
        addIdamError(CODEERRORTYPE, __func__, err, "Unknown function requested!");
    }

    // --------------------------------------------------------------------------------------
    // Housekeeping

    return err;
}

// Help: A Description of library functionality
int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    char* help = PLUGIN_NAME ": this plugin maps HL2A data to IDSs\n\n";
    const char* desc = PLUGIN_NAME ": help = plugin used for mapping HL2A experimental data to IDS";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
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
    UDA_LOG(UDA_LOG_DEBUG, "Calling do_read from HL2A plugin\n");

    int err = 0;

    DATA_BLOCK* data_block = idam_plugin_interface->data_block;

    initDataBlock(data_block);

    data_block->rank = 1;
    data_block->dims = (DIMS*)malloc(data_block->rank * sizeof(DIMS));

    int i;
    for (i = 0; i < data_block->rank; i++) {
        initDimBlock(&data_block->dims[i]);
    }

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    const char* element;    // will contain the UDA mapping got from the UDA request
    int shot;
    int* indices;
    size_t nindices;      

    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);
    FIND_REQUIRED_INT_ARRAY(request_block->nameValueList, indices);

    const char* UDA_MappingKey = element;

    char* mappingFileName = getenv("UDA_HL2A_MAPPING_FILE");

    UDA_LOG(UDA_LOG_DEBUG, "UDA mapping file: %s\n", mappingFileName);
    UDA_LOG(UDA_LOG_DEBUG, "UDA mapping key: %s\n", UDA_MappingKey);

    //Get the mapping function from the value found in the UDA mapping file for the given UDA_MappingKey
    //Get also the IDS type ('static' or 'dynamic')
    int IDS_DataType;
    const char* mapfun = getMappingValue(mappingFileName, UDA_MappingKey, &IDS_DataType);

    //The path requested has not been found
    if (mapfun == NULL) {
        UDA_LOG(UDA_LOG_DEBUG, "The requested function for accessing HL2A data has not been found. Check the UDA mapping file.\n");
        fprintf(stderr, "The requested function for accessing HL2A data has not been found. Check the UDA mapping file.\n");
        addIdamError(CODEERRORTYPE, "no data mapping provided...", 901, "");
        return -1;
    }

    int status = GetHL2AData(shot, mapfun, data_block, indices);
    if (status != 0) {
            return status;
    }

    if (IDS_DataType == STATIC) {
        int data_type = data_block->data_type;

        if (data_type != UDA_TYPE_STRING &&
            data_type != UDA_TYPE_DOUBLE &&
            data_type != UDA_TYPE_FLOAT &&
            data_type != UDA_TYPE_LONG &&
            data_type != UDA_TYPE_INT &&
            data_type != UDA_TYPE_SHORT) {
            err = 999;
            addIdamError(CODEERRORTYPE, __func__, err, "HL2A : Unsupported static data type");
        }

        free(data_block->dims);

        // Scalar data
        data_block->rank   = 0;
        data_block->data_n = 1;

        strcpy(data_block->data_label, "");
        strcpy(data_block->data_units, "");
        strcpy(data_block->data_desc, "");
    }

    return 0;
}



//Get from the UDA mapping file the IDS XPath for the given key and the data type ('static' or 'dynamic')
//Example : <mapping key="antennas/ec/Shape_of" value="//antennas/ec/@dim" type="static"/>
// where the key is 'antennas/ec/Shape_of' and the IDS XPath is '//antennas/ec/@dim', the type is 'static'

static char* getMappingValue(const char* mappingFileName, const char* UDA_MappingKey,
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
        UDA_LOG(UDA_LOG_DEBUG, "Error: unable to parse UDA mapping file\n");
        fprintf(stderr, "Error: unable to parse file \"%s\"\n", mappingFileName);
        return NULL;
    }

    /*
     * Create xpath evaluation context
     */
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        UDA_LOG(UDA_LOG_DEBUG, "Error: unable to create new XPath context\n");
        fprintf(stderr, "Error: unable to create new XPath context\n");
        xmlFreeDoc(doc);
        return NULL;
    }
    // Creating the Xpath request
    UDA_LOG(UDA_LOG_DEBUG, "Creating the Xpath request\n");
    int len = strlen(UDA_MappingKey) + 26;
    xmlChar* xPathExpr = malloc(len + sizeof(xmlChar));
    xmlStrPrintf(xPathExpr, len, "//mapping[@key='%s']/@value", UDA_MappingKey);

    /*
     * Evaluate xpath expression for the type
     */
    xpathObj = xmlXPathEvalExpression(xPathExpr, xpathCtx);
    if (xpathObj == NULL) {
        UDA_LOG(UDA_LOG_DEBUG, "Error: unable to evaluate xpath expression\n");
        fprintf(stderr,
                "Error: unable to evaluate xpath expression \"%s\"\n",
                xPathExpr);
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
        err = 998;
        addIdamError(CODEERRORTYPE, __func__, err, "no result on XPath request");
    }
    xmlStrPrintf(xPathExpr, len, "//mapping[@key='%s']/@type", UDA_MappingKey);

    /*
     * Evaluate xpath expression for the type
     */
    xpathObj = xmlXPathEvalExpression(xPathExpr, xpathCtx);
    if (xpathObj == NULL) {
        UDA_LOG(UDA_LOG_DEBUG,
                 "Error: unable to evaluate xpath expression for getting the type (static or dynamic)\n");
        fprintf(stderr,
                "Error: unable to evaluate xpath expression \"%s\"\n",
                xPathExpr);
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
        err = 998;
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

