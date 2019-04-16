
#ifndef UDA_PLUGIN_HL2A_MAGNETICS_H
#define UDA_PLUGIN_HL2A_MAGNETICS_H

#include <clientserver/udaStructs.h>

void magnetics_ids_properties_homogeneous_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_flux_loop_shape_of(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_flux_loop_position_shape_of(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_flux_loop_position_r(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_flux_loop_position_z(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_flux_loop_flux_data(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_flux_loop_flux_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void magnetics_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);


#endif // UDA_PLUGIN_HL2A_MAGNETICS_H
