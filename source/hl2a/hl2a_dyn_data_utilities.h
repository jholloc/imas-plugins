#ifndef UDA_PLUGIN_HL2A_DYN_DATA_UTILITIES_H
#define UDA_PLUGIN_HL2A_DYN_DATA_UTILITIES_H

#include <clientserver/udaStructs.h>

void SetDynamicData(DATA_BLOCK* data_block, int len, float *time, float *data);

void SetDynamicDataTime(DATA_BLOCK* data_block, int len, float *time, float *data);

void setReturnData2DFloat (DATA_BLOCK* data_block, int dim1_shape, int dim2_shape, float* data);

#endif // UDA_PLUGIN_HL2A_DYN_DATA_UTILITIES_H
