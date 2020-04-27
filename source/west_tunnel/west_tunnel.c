/*---------------------------------------------------------------
 * v1 IDAM Plugin Template: Standardised plugin design template, just add ...
 *
 * Input Arguments:	IDAM_PLUGIN_INTERFACE *idam_plugin_interface
 *
 * Returns:		0 if the plugin functionality was successful
 *			otherwise a Error Code is returned
 *
 * Standard functionality:
 *
 *	help	a description of what this plugin does together with a list of functions available
 *
 *	reset	frees all previously allocated heap, closes file handles and resets all static parameters.
 *		This has the same functionality as setting the housekeeping directive in the plugin interface
 *		data structure to TRUE (1)
 *
 *	init	Initialise the plugin: read all required data and process. Retain staticly for
 *		future reference.
 *
 *---------------------------------------------------------------------------------------------------------------*/
#include "west_tunnel.h"

#include <stdlib.h>
#include <strings.h>

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <plugins/udaPlugin.h>
#include <client/makeClientRequestBlock.h>
#include <client/udaClient.h>
#include <client/getEnvironment.h>
#include <client/accAPI.h>

#include "west_tunnel_ssh.h"
#include "west_tunnel_ssh_server.h"

static int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int open_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int close_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int begin_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int end_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int write_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int read_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int delete_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);
static int begin_arraystruct_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

static int forwardRequest(IDAM_PLUGIN_INTERFACE* idam_plugin_interface);

typedef struct ServerThreadData {
	const char* experiment;
	const char* ssh_host;
	const char* uda_host;
} SERVER_THREAD_DATA;

static void* server_task(void* ptr)
{
	SERVER_THREAD_DATA* data = (SERVER_THREAD_DATA*)ptr;
	ssh_run_server(data->experiment, data->ssh_host, data->uda_host);
	return NULL;
}

int west_tunnel(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	fprintf(stdout, "Calling WEST_TUNNEL plugin\n");
	static int init = 0;

	//----------------------------------------------------------------------------------------
	// Standard v1 Plugin Interface

	if (idam_plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
		RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
	}

	idam_plugin_interface->pluginVersion = THISPLUGIN_VERSION;

	REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;

	if (idam_plugin_interface->housekeeping || STR_IEQUALS(request_block->function, "reset")) {
		if (!init) return 0; // Not previously initialised: Nothing to do!
		// Free Heap & reset counters
		init = 0;
		return 0;
	}

	//----------------------------------------------------------------------------------------
	// Initialise
	if (!init) {
		g_west_tunnel_server_port = 0;
		g_west_tunnel_initialised = false;

		pthread_cond_init(&g_west_tunnel_initialised_cond, NULL);
		pthread_mutex_init(&g_west_tunnel_initialised_mutex, NULL);

		pthread_t server_thread;
		SERVER_THREAD_DATA thread_data = {};
		thread_data.experiment = "WEST";
		thread_data.ssh_host = "gemma.intra.cea.fr";
		thread_data.uda_host = "gemma.intra.cea.fr";

		pthread_create(&server_thread, NULL, server_task, &thread_data);

		pthread_mutex_lock(&g_west_tunnel_initialised_mutex);
		while (!g_west_tunnel_initialised) {
			pthread_cond_wait(&g_west_tunnel_initialised_cond, &g_west_tunnel_initialised_mutex);
		}
		pthread_mutex_unlock(&g_west_tunnel_initialised_mutex);

		pthread_mutex_destroy(&g_west_tunnel_initialised_mutex);
		pthread_cond_destroy(&g_west_tunnel_initialised_cond);

		struct timespec sleep_for;
		sleep_for.tv_sec = 0;
		sleep_for.tv_nsec = 100000000;
		nanosleep(&sleep_for, NULL);

		init = 1;
	}

	if (STR_IEQUALS(request_block->function, "help")) {
		return do_help(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "version")) {
		return do_version(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "builddate")) {
		return do_builddate(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "defaultmethod")) {
		return do_defaultmethod(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "maxinterfaceversion")) {
		return do_maxinterfaceversion(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "openPulse")) {
		return open_pulse(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "closePulse")) {
		return close_pulse(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "beginAction")) {
		return begin_action(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "endAction")) {
		return end_action(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "writeData")) {
		return write_data(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "readData")) {
		return read_data(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "deleteData")) {
		return delete_data(idam_plugin_interface);
	} else if (STR_IEQUALS(request_block->function, "beginArraystructAction")) {
		return begin_arraystruct_action(idam_plugin_interface);
	} else {
		RAISE_PLUGIN_ERROR("Unknown function requested!");
	}
	return 0;
}

int forwardRequest(IDAM_PLUGIN_INTERFACE* idam_plugin_interface) {
	REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;
	REQUEST_BLOCK new_request_block;
	initRequestBlock(&new_request_block);
	int err = 0;

	char request[1024];
	sprintf(request,"IMAS_REMOTE::%s",request_block->signal);
	//printf(stdout, "forwarded request: %s\n", request_block->signal);

	UDA_LOG(UDA_LOG_DEBUG,"forwarded request: %s\n", request);

	if ((err = makeClientRequestBlock(request, "", &new_request_block)) != 0) {
		fprintf(stderr, "failed to create request block");
		return err;
	}
	int handle = idamClient(&new_request_block);
	if (handle < 0) {
		fprintf(stderr, "UDA call failed\n");
		return handle;
	}

	*idam_plugin_interface->data_block = *getIdamDataBlock(handle);
	return 0;
}

/**
 * Help: A Description of library functionality
 * @param idam_plugin_interface
 * @return
 */
int do_help(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
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
int do_version(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param idam_plugin_interface
 * @return
 */
int do_builddate(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return setReturnDataString(idam_plugin_interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param idam_plugin_interface
 * @return
 */
int do_defaultmethod(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return setReturnDataString(idam_plugin_interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param idam_plugin_interface
 * @return
 */
int do_maxinterfaceversion(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return setReturnDataIntScalar(idam_plugin_interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}

int open_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	UDA_LOG(UDA_LOG_DEBUG, "%s", "Opening pulse file in west_tunnel");
	//printf("%s\n", "Executing west_tunnel");
	setenv("UDA_HOST", "localhost", 1);

	char port[100];
	sprintf(port, "%d", g_west_tunnel_server_port);
	setenv("UDA_PORT", port, 1);

	UDA_LOG(UDA_LOG_DEBUG, "%s", "Calling open_pulse of west_tunnel plugin");

	REQUEST_BLOCK* request_block = idam_plugin_interface->request_block;
	const char* tokamak;
	FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, tokamak);

	if (strcmp(tokamak,"WEST") == 0 || strcmp(tokamak,"west") == 0) {

		int backend_id;
		FIND_REQUIRED_INT_VALUE(request_block->nameValueList, backend_id);

		int shot;
		FIND_REQUIRED_INT_VALUE(request_block->nameValueList, shot);

		int run;
		FIND_REQUIRED_INT_VALUE(request_block->nameValueList, run);

		const char* user;
		FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, user);

		const char* version;
		FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, version);

		int mode;
		FIND_REQUIRED_INT_VALUE(request_block->nameValueList, mode);

		const char* options;
		FIND_REQUIRED_STRING_VALUE(request_block->nameValueList, options);

		char request[1024];
		sprintf(request,"openPulse(backend_id=%d, shot=%d, run=%d, user=%s, tokamak=%s, version=%s, mode=%d, options='%s')",
				backend_id, shot, run, "imas_public", "west", version, mode, options);

		strcpy(request_block->signal, request);

	}

	int fr = forwardRequest(idam_plugin_interface);
	return fr;
}

int close_pulse(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return forwardRequest(idam_plugin_interface);
}

int begin_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	fprintf(stdout, "forwarding request begin_action...\n");
	return forwardRequest(idam_plugin_interface);
}

int end_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return forwardRequest(idam_plugin_interface);
}

int write_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return forwardRequest(idam_plugin_interface);
}

int read_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	UDA_LOG(UDA_LOG_DEBUG, "%s", "Executing begin_action in west_tunnel");
	printf("%s\n", "Executing begin_action in west_tunnel");
	setenv("UDA_HOST", "localhost", 1);

	char port[100];
	sprintf(port, "%d", g_west_tunnel_server_port);
	setenv("UDA_PORT", port, 1);
	return forwardRequest(idam_plugin_interface);
}

int delete_data(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return forwardRequest(idam_plugin_interface);
}

int begin_arraystruct_action(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
	return forwardRequest(idam_plugin_interface);
}
