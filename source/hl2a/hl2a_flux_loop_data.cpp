#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <logging/logging.h>
#include <plugins/udaPlugin.h>
#include <clientserver/initStructs.h>
#include <clientserver/errorLog.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <list>

#include "hl2a_flux_loop_data.h"


#include "HL2AResult.h"
#include "HL2ATranslator.h"
#include "TranslatorFactory.h"
#include "DataManagerFactory.h"


//
using namespace HL2A;

void magnetics_flux_loop_xdata(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
	printf("=========================================\r\n");
	printf("==Just For Test xdata today.==\r\n");
	printf("=========================================\r\n");
	setReturnDataIntScalar(data_block, 1, NULL); //returns 1, homogeneous time base
}

void magnetics_flux_loop_ydata(int shotNumber, DATA_BLOCK* data_block, char * system, char * channel)
{
	//Invoke factory to create translator.
	TranslatorFactory factory;
	ITranslator * translator = factory.GetTranslator();

	char key[10];
	//Composit Data File Name
	sprintf(key, "%d%s", shotNumber, system);

	list<HL2A_DATA> listData;
	//Translate data and fill in data in list Data.

	printf("\r=============ready for get the hl2a data=====================\n");

	HL2AResult result = (HL2AResult)translator->DoTranslateWithOutputParam(key, (unsigned char*)&listData, (DataManagerFactory()).GetDataManager());

	printf("\r=============finished get the hl2a data=====================\n");

	int x = result.GetStatus();
	//Verify the status.
	if (x >= 0)
	{
		printf("\r=============ready for write interface data block=====================\n");

		bool isFind = false;
		//Loop the list data.
		for (list<HL2A_DATA>::const_iterator iter = listData.begin(); iter != listData.end(); iter++)
		{
			

			
			//Compare the channel 
			if (strcmp(channel, iter->chnl) == 0)
			{
				printf("\r\n==================================================================\r\n");
				printf("\r\n system=[%s]  channel = [%s] data type=[%d]  \r\n", system, iter->chnl, iter->dtType);
				
				printf("\r\n==================================================================\r\n");
				//output special channel data.
				setReturnHL2AData(data_block, (char*)iter->chnl, iter->chnl_id, iter->dtCnt, iter->dtType, iter->xData, iter->yData, iter->yDataLength);

				isFind = true;
			}
			
		}
		
	}
	printf("\r=============finished the magnetics_flux_loop_ydata function=====================\n");

	free(translator);

}

void setReturnHL2AData(DATA_BLOCK* data_block, char * channel, int channelid, int dataCount, int dtType, void * xDatas, void * yDatas, int yDataLength)
{
	
	initDataBlock(data_block);
	data_block->rank = 1;

	switch (dtType)
	{
	case 1:
		data_block->data_type = UDA_TYPE_SHORT;
		break;
	case 2:
	case 4:
		data_block->data_type = UDA_TYPE_FLOAT;
		break;
	case 3:
		data_block->data_type = UDA_TYPE_INT;
		break;

	}
	
	data_block->data_n = dataCount;
	data_block->data = (char*)yDatas;

	const size_t N = dataCount;

	data_block->dims = (DIMS*)malloc(data_block->rank * sizeof(DIMS));

	data_block->dims[0].data_type = UDA_TYPE_FLOAT;
	data_block->dims[0].dim_n = dataCount;
	

	data_block->dims[0].dim = (char*)xDatas;
	data_block->dims[0].compressed = 1;
	data_block->dims[0].dim0 = 0.0;
	data_block->dims[0].diff = 1.0;
	data_block->dims[0].method = 0;


	strcpy(data_block->data_label, "");
	strcpy(data_block->data_units, "");
	strcpy(data_block->data_desc, "");

}
