#include "hl2a_dyn_data_utilities.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <logging/logging.h>
#include <clientserver/initStructs.h>
#include <clientserver/errorLog.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>

void SetDynData(DATA_BLOCK* data_block, int len, float* time, float* data, int setTime);

void SetDynamicData(DATA_BLOCK* data_block, int len, float* time, float* data)
{
    SetDynData(data_block, len, time, data, 0);
}

void SetDynamicDataTime(DATA_BLOCK* data_block, int len, float* time, float* data)
{
    SetDynData(data_block, len, time, data, 1);
}

void SetDynData(DATA_BLOCK* data_block, int len, float* time, float* data, int setTime)
{

    //IDAM data block initialization
    initDataBlock(data_block);
    int i;
    data_block->rank = 1;
    data_block->data_type = UDA_TYPE_FLOAT;
    data_block->data_n = len;

    if (setTime == 0) {
        data_block->data = (char*)data;
    } else {
        data_block->data = (char*)time;
    }

    data_block->dims = (DIMS*)malloc(data_block->rank * sizeof(DIMS));

    for (i = 0; i < data_block->rank; i++) {
        initDimBlock(&data_block->dims[i]);
    }

    data_block->dims[0].data_type = UDA_TYPE_FLOAT;
    data_block->dims[0].dim_n = len;
    data_block->dims[0].compressed = 0;
    data_block->dims[0].dim = (char*)time;

    strcpy(data_block->data_label, "");
    strcpy(data_block->data_units, "");
    strcpy(data_block->data_desc, "");
}

void setReturnData2DFloat (DATA_BLOCK* data_block, int dim1_shape, int dim2_shape, float* data)
{
	//IDAM data block initialization
	initDataBlock(data_block);
	data_block->rank = 2;
	data_block->dims = (DIMS*)malloc(data_block->rank * sizeof(DIMS));

	int i;
	for (i = 0; i < data_block->rank; i++) {
		initDimBlock(&data_block->dims[i]);
		data_block->dims[i].data_type = UDA_TYPE_UNSIGNED_INT;
		data_block->dims[i].compressed = 1;
		data_block->dims[i].dim0 = 0.0;
		data_block->dims[i].diff = 1.0;
		data_block->dims[i].method = 0;
	}
	data_block->data_type = UDA_TYPE_FLOAT;
	data_block->dims[0].dim_n = dim1_shape;
	data_block->dims[1].dim_n = dim2_shape;
	data_block->data_n = dim1_shape*dim2_shape;
	data_block->data = (char*)data;
}

