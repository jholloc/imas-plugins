#include "hl2a_mapped_data.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <logging/logging.h>
#include <clientserver/initStructs.h>
#include <clientserver/errorLog.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>

#include "hl2a_magnetics.h"
#include "hl2a_soft_x_rays.h"

void getFunName(const char* s, char** fun_name);
void RemoveSpaces(char* source);

int GetHL2AData(int shotNumber, const char* mapfun, DATA_BLOCK* data_block, int* nodeIndices)
{
    int status = -1;
    UDA_LOG(UDA_LOG_DEBUG, "Entering GetHL2AData() -- HL2A plugin\n");
    assert(mapfun); //Mandatory
    char* fun_name = NULL;
    getFunName(mapfun, &fun_name);

    UDA_LOG(UDA_LOG_DEBUG, "UDA request: %s\n", fun_name);


    if (strcmp(fun_name, "magnetics_ids_properties_homogeneous_time") == 0) {
        magnetics_ids_properties_homogeneous_time(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_flux_loop_shape_of") == 0) {
        magnetics_flux_loop_shape_of(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_flux_loop_position_shape_of") == 0) {
        magnetics_flux_loop_position_shape_of(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_flux_loop_position_r") == 0) {
        magnetics_flux_loop_position_r(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_flux_loop_position_z") == 0) {
        magnetics_flux_loop_position_z(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_flux_loop_flux_data") == 0) {
        magnetics_flux_loop_flux_data(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_flux_loop_flux_time") == 0) {
        magnetics_flux_loop_flux_time(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "magnetics_time") == 0) {
        magnetics_time(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "soft_x_rays_ids_properties_homogeneous_time") == 0) {
        soft_x_rays_ids_properties_homogeneous_time(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "soft_x_rays_time") == 0) {
        soft_x_rays_time(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "soft_x_rays_channel_shape_of") == 0) {
        soft_x_rays_channel_shape_of(shotNumber, data_block, nodeIndices);
    } else if (strcmp(fun_name, "soft_x_rays_channel_power_density_data") == 0) {
        soft_x_rays_channel_power_density_data(shotNumber, data_block, nodeIndices);
    } else {
        const char* errorMsg = "HL2A:ERROR: mapped C function not found in hl2a_mapped_data.c";
	    UDA_LOG(UDA_LOG_DEBUG, "%s:%s\n", errorMsg, fun_name);
	    UDA_LOG(UDA_LOG_ERROR, "%s:%s\n", errorMsg, fun_name);
        return status;
      }
    free(fun_name);
    status = 0;
    return status;
}

void getFunName(const char* s, char** fun_name)
{
	const char delim[] = ";";
	char* s_copy = strdup(s);
	*fun_name = strdup(strtok(s_copy, delim));
	RemoveSpaces(*fun_name);
	free(s_copy);
}

void RemoveSpaces(char* source)
{
	char* i = source;
	char* j = source;
	while (*j != 0) {
		*i = *j++;
		if (*i != ' ') {
			i++;
		}
	}
	*i = 0;
}

