#include "hl2a_magnetics.h"

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



void magnetics_ids_properties_homogeneous_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    setReturnDataIntScalar(data_block, 1, NULL); //returns 1, homogeneous time base
}

void magnetics_flux_loop_shape_of(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
   int N = 10;
   setReturnDataIntScalar(data_block, N, NULL); //returns N flux loops
}

void magnetics_flux_loop_position_shape_of(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
   int N = 3;
   setReturnDataIntScalar(data_block, N, NULL); //returns N positions
}

void magnetics_flux_loop_position_r(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
   int flux_loop_id = nodeIndices[0]; //starts from 1
   int position_id  = nodeIndices[1]; //starts from 1
   float r = 0.;

   if (flux_loop_id < 6) {
	   if (position_id == 1) {
	       r = 10.;
	   }
	   else if (position_id == 2) {
	       r = 12.;
	   }
	   else if (position_id == 3) {
	       r = 14.;
	   }
    }
    else if (flux_loop_id >= 6) {
          if (position_id == 1) {
	       r = 20.;
	   }
	   else if (position_id == 2) {
	       r = 22.;
	   }
	   else if (position_id == 3) {
	       r = 24.;
	   }
   }

   setReturnDataFloatScalar(data_block, r, NULL); //returns r position
}

void magnetics_flux_loop_position_z(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
   int flux_loop_id = nodeIndices[0]; //starts from 1
   int position_id  = nodeIndices[1]; //starts from 1
   float z = 0.;

   if (flux_loop_id < 6) {
	   if (position_id == 1) {
	       z = 100.;
	   }
	   else if (position_id == 2) {
	       z = 120.;
	   }
	   else if (position_id == 3) {
	       z = 140.;
	   }
    }
    else if (flux_loop_id >= 6) {
          if (position_id == 1) {
	       z = 200.;
	   }
	   else if (position_id == 2) {
	       z = 220.;
	   }
	   else if (position_id == 3) {
	       z = 240.;
	   }
   }

   setReturnDataFloatScalar(data_block, z, NULL); //returns z position
}



void magnetics_flux_loop_flux_data(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    const size_t N= 5; //number of data points
    float* data = malloc(sizeof(float)*N);
    size_t i = 0;
    for (i = 0; i < N; i++) {
       float fi = (float) i;
       data[i] = 2.*fi;
    }
    const size_t* shape = &N;
    setReturnDataFloatArray(data_block, data, 1, shape, "");
}

void magnetics_flux_loop_flux_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    const size_t N= 5; //number of data points
    float* time = malloc(sizeof(float)*N);
    size_t i = 0;
    for (i = 0; i < N; i++) {
       time[i] = (float) i;
    }
    const size_t* shape = &N;
    setReturnDataFloatArray(data_block, time, 1, shape, ""); 
}

void magnetics_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
    const size_t N= 5; //number of data points
    float* time = malloc(sizeof(float)*N);
    size_t i = 0;
    for (i = 0; i < N; i++) {
       time[i] = (float) i;
    }
    const size_t* shape = &N;
    setReturnDataFloatArray(data_block, time, 1, shape, ""); 
}

