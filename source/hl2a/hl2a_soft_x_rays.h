
#ifndef IDAM_PLUGIN_HL2A_SOFT_X_RAYS_H
#define IDAM_PLUGIN_HL2A_SOFT_X_RAYS_H

#include <clientserver/udaStructs.h>

void soft_x_rays_ids_properties_homogeneous_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void soft_x_rays_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void soft_x_rays_channel_shape_of(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void soft_x_rays_channel_power_density_data(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);

#endif // IDAM_PLUGIN_HL2A_SOFT_X_RAYS_H
