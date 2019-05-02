#include "west_ece.h"

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

#include "west_ece_mode.h"
#include "west_utilities.h"
#include "west_dyn_data_utilities.h"
#include "ts_rqparam.h"

int SHOT_52374 = 52374;

void ece_throwsIdamError(int status, char* methodName, char* object_name, int shotNumber) {
	int err = 901;
	char msg[1000];
	sprintf(msg, "%s(%s),object:%s,shot:%d,err:%d\n", "WEST:ERROR", methodName, object_name, shotNumber,status);
	//UDA_LOG(UDA_LOG_ERROR, "%s", msg);
	addIdamError(CODEERRORTYPE, msg, err, "");
}

void ece_throwsIdamError2(int status, char* methodName, char* object_name, int channel, int shotNumber) {
	int err = 901;
	char msg[1000];
	sprintf(msg, "%s(%s),object:%s,shot:%d,channel:%d,err:%d\n", "WEST:ERROR", methodName, object_name, shotNumber,channel,status);
	//UDA_LOG(UDA_LOG_ERROR, "%s", msg);
	addIdamError(CODEERRORTYPE, msg, err, "");
}

int homogeneous_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices){
	int homogeneous_time = 1;
	if (shotNumber < SHOT_52374) {
		homogeneous_time = -1;
	}
	if (homogeneous_time == -1) {//an error has occurred, we do not set the field
		ece_throwsIdamError(-1, "homogeneous_time", "", shotNumber);
		return -1;
	}
	setReturnDataIntScalar(data_block, homogeneous_time, NULL);
	return 0;
}

int ece_t_e_data_shape_of(int shotNumber, char** mapfun)
{
	if (shotNumber < SHOT_52374) {
		ece_throwsIdamError(-1, "ece_t_e_data_shape_of", "", shotNumber);
		return -1;
	}
	*mapfun = strdup("shape_of_tsmat_collect;DVECE:GVSH1:VOIE,DVECE:GVSH2:VOIE,DVECE:GVSH3:VOIE,DVECE:GVSH4:VOIE;0:float:#0");
	return 0;
}

int ece_t_e_data(int shotNumber, char** mapfun)
{
	if (shotNumber < SHOT_52374) {
		ece_throwsIdamError(-1, "ece_t_e_data", "", shotNumber);
		return -1;
	}
	*mapfun = strdup("tsbase_collect;DVECE:GVSH1,DVECE:GVSH2,DVECE:GVSH3,DVECE:GVSH4;1:float:#0");
	return 0;
}

int ece_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices)
{
	if (shotNumber < SHOT_52374) {
		ece_throwsIdamError(-1, "ece_t_e_data", "", shotNumber);
		return -1;
	}

	//Time is homogeneous, we take for example GVSH1%1
	int status = setUDABlockTimeFromArcade("GVSH1", shotNumber, 1, data_block, nodeIndices);
	if (status != 0) {
		ece_throwsIdamError(status, "ece_time", "GVSH1", shotNumber);
		return -1;
	}

	return 0;
}

