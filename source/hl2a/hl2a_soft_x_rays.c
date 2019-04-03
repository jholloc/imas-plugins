#include "hl2a_soft_x_rays.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <logging/logging.h>
#include <clientserver/initStructs.h>
#include <clientserver/errorLog.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <plugins/udaPlugin.h>

#include "hl2a_dyn_data_utilities.h"

void test2D_hl2a(float* data,  size_t* shape1,  size_t* shape2);

void soft_x_rays_ids_properties_homogeneous_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    setReturnDataIntScalar(data_block, 1, NULL); //returns 1, homogeneous time base
}

void soft_x_rays_channel_shape_of(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
   int N = 10;
   setReturnDataIntScalar(data_block, N, NULL); //returns N flux loops
}

void soft_x_rays_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    const size_t N= 1000; //number of data points
    float* time = malloc(sizeof(float)*N);
    size_t i = 0;
    for (i = 0; i < N; i++) {
       time[i] = (float) i;
    }
    const size_t* shape = &N;
    setReturnDataFloatArray(data_block, time, 1, shape, ""); 
}

void soft_x_rays_channel_power_density_data(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    UDA_LOG(UDA_LOG_DEBUG, "Calling soft_x_rays_channel_power_density_data from HL2A plugin\n");
    float* data = NULL;
    
    size_t shape1Int = 10;
    size_t shape2Int = 10;
    data = malloc(sizeof(float)*shape1Int*shape2Int);
    test2D_hl2a(data, &shape1Int, &shape2Int);
    
    UDA_LOG(UDA_LOG_DEBUG, "after Calling test2D_hl2a from HL2A plugin\n");
    size_t shapeInt[2];
    shapeInt[0] = shape1Int;
    shapeInt[1] = shape2Int;
    const size_t* shape = shapeInt;
    UDA_LOG(UDA_LOG_DEBUG, "after Calling test2D_hl2a from HL2A plugin, size of shape: %d %d\n", shapeInt[0], shapeInt[1]);
    
    UDA_LOG(UDA_LOG_DEBUG, "after Calling test2D_hl2a from HL2A plugin, data: %f %f %f\n", data[0], data[1], data[shapeInt[0]*shapeInt[1]-1]);
    //setReturnDataFloatArray(data_block, data, 2, shape, "");
    setReturnData2DFloat(data_block, 10, 10, data);
    UDA_LOG(UDA_LOG_DEBUG, "returning from setReturnDataFloatArray from HL2A plugin\n");
}

void test2D_hl2a(float* data,  size_t* shape1,  size_t* shape2) 
{
   const size_t N1 = 10;
   const size_t N2 = 10;
   size_t i, j;
   for (i = 0; i < N1; i++) {
        for (j = 0; j < N2; j++) {
            data[i*N2+j] = (float)(i+j);
         }
   }
   *shape1 = N1;
   *shape2 = N2;
}




