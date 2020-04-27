#include "west_polaro_interf.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <logging/logging.h>
#include <clientserver/initStructs.h>
#include <clientserver/errorLog.h>
#include <clientserver/stringUtils.h>
#include <clientserver/udaTypes.h>
#include <plugins/udaPlugin.h>
#include <structures/struct.h>

#include "west_utilities.h"
#include "west_dyn_data_utilities.h"
#include "ts_rqparam.h"
#include "west_static_data_utilities.h"

int polaro_or_interfero_channel_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	int channelID = nodeIndices[0]; //starts from 1
	char channel_name[10];
	sprintf(channel_name, "%d", channelID);
	setReturnDataString(data_block, &channel_name[0], NULL);
	return 0;
}
