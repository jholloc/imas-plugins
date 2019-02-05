#include "tcvm_plugin.h"

#include <string.h>
#include <assert.h>
#include <float.h>

#include <clientserver/errorLog.h>
#include <clientserver/initStructs.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <logging/logging.h>
#include <plugins/udaPlugin.h>

#include "testStart.h"

int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
char* deblank(char* token);


int tcvmPlugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    // ----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

    REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

    static short init = 0;

    // ----------------------------------------------------------------------------------------
    // Heap Housekeeping

    if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
        if (!init) {
            // Not previously initialised: Nothing to do!
            return 0;
        }

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

    int err = 0;

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
        RAISE_PLUGIN_ERROR("Unknown function requested!");
    }

    return err;
}

// Help: A Description of library functionality
int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* help = "\ntcvmPlugin: this plugin maps TCV data to IDS using MATLAB\n\n";
    const char* desc = "tcvmPlugin: help = plugin used for mapping TCV data to IDS using MATLAB";

    return setReturnDataString(idam_plugin_interface->data_block, help, desc);
}

int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin version number";

    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, desc);
}

// Plugin Build Date
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin build date";

    return setReturnDataString(idam_plugin_interface->data_block, __DATE__, desc);
}

// Plugin Default Method
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Plugin default method";

    return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, desc);
}

// Plugin Maximum Interface Version
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    const char* desc = "Maximum Interface Version";

    return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, desc);
}

// ----------------------------------------------------------------------------------------
// Add functionality here ....
int do_read(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
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

    const char* element;        // will contain the modified IDSRequest
    // which will be one key of the IDS
    // requests mapping file
    int shot;
    int* indices;
    size_t nindices;
    int dtype;

    FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, element);
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);
    FIND_REQUIRED_INT_ARRAY(request_block->nameValueList, indices);
    FIND_REQUIRED_INT_VALUE(request_block->nameValueList, dtype);


    static bool first_run = true;
    static Engine *matlabPtr = NULL;
    
    if (first_run || !matlabPtr) {
        // Start new MATLAB session
        matlabPtr = mystartMATLAB();
	if (matlabPtr) {
	  fprintf(stdout,"Started MATLAB session\n");
        } else {
	  fprintf(stdout,"Failed to start MATLAB session\n");
        }
    } else {
        fprintf(stdout,"Pointer to existing session still alive\n");
    }

    if (matlabPtr) {
        testMATLAB(matlabPtr);
	char *result;
	runquery(matlabPtr,element,&result);

	free(data_block->dims);
	data_block->dims = NULL;
	
	data_block->rank = 0;
	data_block->data_type = UDA_TYPE_STRING;
	data_block->data_n = 1;
	data_block->data = strdup(result);
	
	strcpy(data_block->data_label, "");
	strcpy(data_block->data_units, "");
	strcpy(data_block->data_desc, "");

        //matlab::data::ArrayFactory factory;
        
        //matlab::data::TypedArray<int32_t> indices_a = factory.createArray<int32_t>({ 1,nindices }, {});
	//int i = 0;
	//for (auto& elem : indices_a) {
        //    elem = (* indices + i++);
	//}
	//std::vector<matlab::data::Array> args({
	//    factory.createCharArray(element),
	//    factory.createScalar<double>(shot),
	      //   indices_a,
	//    factory.createScalar<int32_t>(dtype)
        //});
        //matlab::data::CharArray out = matlabPtr->feval(u"myFunction", args);
	//std::cout << "Answer to " << element << " is " << out.toAscii() << std::endl;
    }

    // Avoids looking twice for sessions on startup
    first_run = false;




    return err;
}
