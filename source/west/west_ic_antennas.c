#include "west_ic_antennas.h"

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

void ic_antennas_throwsIdamError(int status, char* methodName, char * objectName, int shotNumber);
void ic_antennas_throwsIdamError1(int status, char* methodName, char * objectName, int shotNumber, int antennaId);
void ic_antennas_throwsIdamError2(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId, int matching_element);
void ic_antennas_throwsIdamError3(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId, int currentId);
void ic_antennas_throwsIdamError4(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId, int voltageId);
void ic_antennas_throwsIdamError5(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId);

void ic_antennas_throwsIdamError(int status, char* methodName, char * objectName, int shotNumber) {
	int err = 901;
	char errorMsg[1000];
	sprintf(errorMsg, "%s(%s),object:%s,shot:%d,err:%d\n", "WEST:ERROR", methodName, objectName, shotNumber,status);
	addIdamError(CODEERRORTYPE, errorMsg, err, "");
}

void ic_antennas_throwsIdamError1(int status, char* methodName, char * objectName, int shotNumber, int antennaId) {
	int err = 901;
	char errorMsg[1000];
	sprintf(errorMsg, "%s(%s),object:%s,shot:%d,antennaId:%d,err:%d\n", "WEST:ERROR", methodName, objectName, shotNumber, antennaId, status);
	addIdamError(CODEERRORTYPE, errorMsg, err, "");
}

void ic_antennas_throwsIdamError2(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId, int matching_element) {
	int err = 901;
	char errorMsg[1000];
	sprintf(errorMsg, "%s(%s),object:%s,shot:%d,antenna:%d,module:%d,matching_element:%d,err:%d\n", "WEST:ERROR", methodName, objectName, shotNumber,antennaId, moduleId, matching_element, status);
	addIdamError(CODEERRORTYPE, errorMsg, err, "");
}

void ic_antennas_throwsIdamError3(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId, int currentId) {
	int err = 901;
	char errorMsg[1000];
	sprintf(errorMsg, "%s(%s),object:%s,shot:%d,antenna:%d,module:%d,current:%d,err:%d\n", "WEST:ERROR", methodName, objectName, shotNumber,antennaId, moduleId, currentId, status);
	addIdamError(CODEERRORTYPE, errorMsg, err, "");
}

void ic_antennas_throwsIdamError4(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId, int voltageId) {
	int err = 901;
	char errorMsg[1000];
	sprintf(errorMsg, "%s(%s),object:%s,shot:%d,antenna:%d,module:%d,voltage:%d,err:%d\n", "WEST:ERROR", methodName, objectName, shotNumber,antennaId, moduleId, voltageId, status);
	addIdamError(CODEERRORTYPE, errorMsg, err, "");
}

void ic_antennas_throwsIdamError5(int status, char* methodName, char * objectName, int shotNumber, int antennaId, int moduleId) {
	int err = 901;
	char errorMsg[1000];
	sprintf(errorMsg, "%s(%s),object:%s,shot:%d,antenna:%d,module:%d,err:%d\n", "WEST:ERROR", methodName, objectName, shotNumber,antennaId, moduleId, status);
	addIdamError(CODEERRORTYPE, errorMsg, err, "");
}

int ic_antennas_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	int antennaId = nodeIndices[0];
	if (antennaId == 1) {
		const char* name = "Q1";
		setReturnDataString(data_block, name, NULL);
	}
	else if (antennaId == 2) {
		const char* name = "Q2";
		setReturnDataString(data_block, name, NULL);
	}
	else if (antennaId == 3) {
		const char* name = "Q4";
		setReturnDataString(data_block, name, NULL);
	}
	return 0;
}

int ic_antennas_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	return ic_antennas_name(shotNumber, data_block, nodeIndices);
}

int ic_antennas_module_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	int moduleId = nodeIndices[1];
	if (moduleId == 1) {
		const char* name = "left";
		setReturnDataString(data_block, name, NULL);
	}
	else if (moduleId == 2) {
		const char* name = "right";
		setReturnDataString(data_block, name, NULL);
	}
	return 0;
}

int ic_antennas_module_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	return ic_antennas_module_name(shotNumber, data_block, nodeIndices);
}

int ic_antennas_module_pressure_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];

	int cases[3][2] = {1,2,3,4,5,6};

	int c = cases[antennaId - 1][moduleId - 1];

	if (c == 1) {
		const char* name = "Q1 left";
		setReturnDataString(data_block, name, NULL);
	}
	else if (c == 2) {
		const char* name = "Q1 right";
		setReturnDataString(data_block, name, NULL);
	}
	else if (c == 3) {
		const char* name = "Q2 left";
		setReturnDataString(data_block, name, NULL);
	}
	else if (c == 4) {
		const char* name = "Q2 right";
		setReturnDataString(data_block, name, NULL);
	}
	else if (c == 5) {
		const char* name = "Q4 left";
		setReturnDataString(data_block, name, NULL);
	}
	else if (c == 6) {
		const char* name = "Q4 right";
		setReturnDataString(data_block, name, NULL);
	}
	return 0;
}

int ic_antennas_module_pressure_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	return ic_antennas_module_pressure_name(shotNumber, data_block, nodeIndices);
}

/*
 * matching capacitor values [pF] (1: Q1 left upper, 2: Q1 left lower, 3:
 * Q1 right upper, 4:Q1 right lower; 5: Q2 left upper, 6: Q2 left lower, 7:
 * Q2 right upper, 8:Q2 right lower; 9: Q4 left upper, 10: Q4 left lower, 11:
 * Q4 right upper, 12:Q4 right lower) module/matching_element/capacitance
 */

int ic_antennas_module_matching_element_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int matching_element = nodeIndices[2];

	int extractionIndices[3][2][2] = {1,2,3,4,5,6,7,8,9,10,11,12};

	int extractionIndex = extractionIndices[antennaId - 1] [moduleId - 1] [matching_element - 1];
	if (extractionIndex == 1) {
		const char* name = "Q1 left upper";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 2) {
		const char* name = "Q1 left lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 3) {
		const char* name = "Q1 right upper";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 4) {
		const char* name = "Q1 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 5) {
		const char* name = "Q2 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 6) {
		const char* name = "Q2 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 7) {
		const char* name = "Q2 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 8) {
		const char* name = "Q2 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 9) {
		const char* name = "Q4 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 10) {
		const char* name = "Q4 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 11) {
		const char* name = "Q4 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	else if (extractionIndex == 12) {
		const char* name = "Q4 right lower";
		setReturnDataString(data_block, name, NULL);
	}
	return 0;
}

int ic_antennas_module_matching_element_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	return ic_antennas_module_matching_element_name(shotNumber, data_block, nodeIndices);
}

int ic_antennas_module_power_forward(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1e3; //conversion factor, power is given in MW in Arcade
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex = 0;
	if (moduleId == 1) { //left
		extractionIndex = 1;
	}
	else { //right
		extractionIndex = 3;
	}
	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHANTPOWQ1";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHANTPOWQ2";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHANTPOWQ4";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_power_forward", "GICHANTPOWQ1/GICHANTPOWQ2/GICHANTPOWQ4", shotNumber, antennaId, moduleId);
	}
	return status;
}

int ic_antennas_module_power_forward_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex = 0;
	if (moduleId == 1) { //left
		extractionIndex = 1;
	}
	else { //right
		extractionIndex = 3;
	}
	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHANTPOWQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHANTPOWQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHANTPOWQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_module_power_forward_time", "GICHANTPOWQ1/GICHANTPOWQ2/GICHANTPOWQ4", shotNumber, antennaId, moduleId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_power_reflected(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1e3; //conversion factor, power is given in kW in Arcade
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex = 0;
	if (moduleId == 1) { //left
		extractionIndex = 2;
	}
	else { //right
		extractionIndex = 4;
	}
	if (antennaId == 1) {
		char *object_name = "GICHANTPOWQ1";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 2) {
		char *object_name = "GICHANTPOWQ2";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 3) {
		char *object_name = "GICHANTPOWQ4";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_power_reflected","GICHANTPOWQ1/GICHANTPOWQ2/GICHANTPOWQ4", shotNumber, antennaId, moduleId);
	}
	return status;
}

int ic_antennas_module_power_reflected_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex = 0;
	if (moduleId == 1) { //left
		extractionIndex = 2;
	}
	else { //right
		extractionIndex = 4;
	}
	if (antennaId == 1) {
		char *object_name = "GICHANTPOWQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 2) {
		char *object_name = "GICHANTPOWQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 3) {
		char *object_name = "GICHANTPOWQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_power_reflected_time","GICHANTPOWQ1/GICHANTPOWQ2/GICHANTPOWQ4", shotNumber, antennaId, moduleId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_phase_forward(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 3.1415926535897932384626433832795/180.; //conversion factor, from deg to rad
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex = 1;
	if (moduleId == 1) { //left
		//phase = 0, reference
		float *time = NULL;
		float *data = NULL;
		int len;
		if (antennaId == 1) { //Q1 antenna
			char *object_name = "GICHPHASESQ1";
			status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
		}
		else if (antennaId == 2) { //Q2 antenna
			char *object_name = "GICHPHASESQ2";
			status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
		}
		else if (antennaId == 3) { //Q3 antenna
			char *object_name = "GICHPHASESQ4";
			status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
		}
		if (status != 0) {
			ic_antennas_throwsIdamError5(status, "ic_antennas_module_phase_forward", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId);
			return status;
		}
		//We create a null vector with length = len(time)
		data = malloc(sizeof(float)*len);
		int i;
		for (i = 0; i < len; i++)
			data[i] = 0.0;
		SetDynamicData(data_block, len, time, data);
	}
	else {
		if (antennaId == 1) { //Q1 antenna
			char *object_name = "GICHPHASESQ1";
			status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
		}
		else if (antennaId == 2) { //Q2 antenna
			char *object_name = "GICHPHASESQ2";
			status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
		}
		else if (antennaId == 3) { //Q3 antenna
			char *object_name = "GICHPHASESQ4";
			status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
		}
		if (status != 0) {
			ic_antennas_throwsIdamError5(status, "ic_antennas_module_phase_forward", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId);
			return status;
		}
	}
	return 0;
}

int ic_antennas_module_phase_forward_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex = 1;

	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHPHASESQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHPHASESQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHPHASESQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_module_phase_forward_time", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_phase_reflected(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 3.1415926535897932384626433832795/180.; //conversion factor, from deg to rad
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex;
	if (moduleId == 1) { //left
		extractionIndex = 2;
	}
	else { //right
		extractionIndex = 5;
	}
	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHPHASESQ1";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHPHASESQ2";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHPHASESQ4";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_module_phase_reflected", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId);
	}
	return status;
}

int ic_antennas_module_phase_reflected_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;
	int extractionIndex;
	if (moduleId == 1) { //left
		extractionIndex = 2;
	}
	else { //right
		extractionIndex = 5;
	}
	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHPHASESQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHPHASESQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHPHASESQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_module_phase_reflected_time", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_voltage_phase(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 3.1415926535897932384626433832795/180.; //conversion factor, from deg to rad
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int voltageId = nodeIndices[2];
	int status = -1;

	int extractionIndices[2][2] = {3, 4, 6, 7};
	/*extractionIndices[0][0] = 3;
		extractionIndices[0][1] = 4;
		extractionIndices[1][0] = 6;
		extractionIndices[1][1] = 7;*/
	int extractionIndex = extractionIndices[moduleId - 1][voltageId - 1];

	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHPHASESQ1";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHPHASESQ2";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHPHASESQ4";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}

	if (status != 0) {
		ic_antennas_throwsIdamError4(status, "ic_antennas_module_voltage_phase", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId, voltageId);
	}
	return status;
}

int ic_antennas_module_voltage_phase_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int voltageId = nodeIndices[2];
	int status = -1;

	int extractionIndices[2][2] = {3, 4, 6, 7};
	/*extractionIndices[0][0] = 3;
	extractionIndices[0][1] = 4;
	extractionIndices[1][0] = 6;
	extractionIndices[1][1] = 7;*/

	int extractionIndex = extractionIndices[moduleId - 1][voltageId - 1];

	if (antennaId == 1) { //Q1 antenna
		char *object_name = "GICHPHASESQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 2) { //Q2 antenna
		char *object_name = "GICHPHASESQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 3) { //Q3 antenna
		char *object_name = "GICHPHASESQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError4(status, "ic_antennas_module_voltage_phase_time", "GICHPHASESQ1/GICHPHASESQ2/GICHPHASESQ4", shotNumber, antennaId, moduleId, voltageId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_current(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1.0; //conversion factor
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int currentId = nodeIndices[2];
	int status = -1;

	//first index = antennaId
	//second index = moduleId
	//third index = currentId
	int extractionIndices[3][2][2] = {1,2,3,4,5,6,7,8,9,10,11,12};

	//first antenna
	//first module --> 'left'
	//extractionIndices[0,0,0] = 1; //upper
	//extractionIndices[0,0,1] = 2; //lower
	//first antenna
	//second module --> 'right'
	//extractionIndices[0,1,0] = 3; //upper
	//extractionIndices[0,1,1] = 4; //lower
	//second antenna
	//first module --> 'left'
	//extractionIndices[1,0,0] = 5; //upper
	//extractionIndices[1,0,1] = 6; //lower
	//second antenna
	///second module --> 'right'
	//extractionIndices[1,1,0] = 7; //upper
	//extractionIndices[1,1,1] = 8; //lower
	//third antenna
	//first module --> 'left'
	//extractionIndices[2,0,0] = 9; //upper
	//extractionIndices[2,0,1] = 10; //lower
	//third antenna
	//second module --> 'right'
	//extractionIndices[2,1,0] = 11; //upper
	//extractionIndices[2,1,1] = 12; //lower
	//int *ptr_extractionIndices = extractionIndices[0,0,0];
	//int extractionIndex = extractionIndices[antennaId - 1, moduleId - 1, currentId - 1];
	int extractionIndex = extractionIndices[antennaId - 1][moduleId - 1][currentId - 1];
	char *object_name = "GICHICAPA";
	status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);

	if (status != 0) {
		ic_antennas_throwsIdamError3(status, "ic_antennas_module_current","GICHICAPA", shotNumber, antennaId, moduleId, currentId);
	}
	return status;
}

int ic_antennas_module_current_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int currentId = nodeIndices[2];
	int status = -1;

	//first index = antennaId
	//second index = moduleId
	//third index = currentId
	int extractionIndices[3][2][2] = {1,2,3,4,5,6,7,8,9,10,11,12};

	//first antenna
	//first module --> 'left'
	//extractionIndices[0,0,0] = 1; //upper
	//extractionIndices[0,0,1] = 2; //lower
	//first antenna
	//second module --> 'right'
	//extractionIndices[0,1,0] = 3; //upper
	//extractionIndices[0,1,1] = 4; //lower
	//second antenna
	//first module --> 'left'
	//extractionIndices[1,0,0] = 5; //upper
	//extractionIndices[1,0,1] = 6; //lower
	//second antenna
	///second module --> 'right'
	//extractionIndices[1,1,0] = 7; //upper
	//extractionIndices[1,1,1] = 8; //lower
	//third antenna
	//first module --> 'left'
	//extractionIndices[2,0,0] = 9; //upper
	//extractionIndices[2,0,1] = 10; //lower
	//third antenna
	//second module --> 'right'
	//extractionIndices[2,1,0] = 11; //upper
	//extractionIndices[2,1,1] = 12; //lower

	int extractionIndex = extractionIndices[antennaId - 1][moduleId - 1][currentId - 1];
	char *object_name = "GICHICAPA";
	status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);

	if (status != 0) {
		ic_antennas_throwsIdamError3(status, "ic_antennas_module_current_time","GICHICAPA", shotNumber, antennaId, moduleId, currentId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_voltage(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1.e3; //conversion factor from kV to V
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int voltageId = nodeIndices[2];
	int status = -1;

	//first index = moduleId
	//second index = voltageId
	int extractionIndices[2][2] = {1,2,3,4};
	//first module --> 'left'
	//extractionIndices[0,0] = 1; //upper
	//extractionIndices[0,1] = 2; //lower
	//second module --> 'right'
	//extractionIndices[1,0] = 3; //upper
	//extractionIndices[1,1] = 4; //lower

	int extractionIndex = extractionIndices[moduleId - 1][voltageId - 1];

	if (antennaId == 1) {
		char *object_name = "GICHVPROBEQ1";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 2) {
		char *object_name = "GICHVPROBEQ2";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	else if (antennaId == 3) {
		char *object_name = "GICHVPROBEQ4";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError4(status, "ic_antennas_module_voltage","GICHVPROBEQ1/GICHVPROBEQ2/GICHVPROBEQ4", shotNumber, antennaId, moduleId, voltageId);
	}
	return status;
}

int ic_antennas_module_voltage_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int voltageId = nodeIndices[2];
	int status = -1;

	//first index = moduleId
	//second index = voltageId
	int extractionIndices[2][2] = {1,2,3,4};
	//first module --> 'left'
	//extractionIndices[0,0] = 1; //upper
	//extractionIndices[0,1] = 2; //lower
	//second module --> 'right'
	//extractionIndices[1,0] = 3; //upper
	//extractionIndices[1,1] = 4; //lower
	//int *ptr_extractionIndices = extractionIndices[0,0];
	int extractionIndex = extractionIndices[moduleId - 1][voltageId - 1];

	if (antennaId == 1) {
		char *object_name = "GICHVPROBEQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 2) {
		char *object_name = "GICHVPROBEQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	else if (antennaId == 3) {
		char *object_name = "GICHVPROBEQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}
	if (status != 0) {
		ic_antennas_throwsIdamError4(status, "ic_antennas_module_voltage","GICHVPROBEQ1/GICHVPROBEQ2/GICHVPROBEQ4", shotNumber, antennaId, moduleId, voltageId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_matching_element_capacity(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1.e-12; //conversion factor
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int matching_element = nodeIndices[2];
	int status = -1;

	//first index = antennaId
	//second index = moduleId
	//third index = matching_element
	int extractionIndices[3][2][2] = {1,2,3,4,5,6,7,8,9,10,11,12};

	//first antenna
	//first module --> 'left'
	//extractionIndices[0,0,0] = 1; //upper
	//extractionIndices[0,0,1] = 2; //lower
	//first antenna
	//second module --> 'right'
	//extractionIndices[0,1,0] = 3; //upper
	//extractionIndices[0,1,1] = 4; //lower
	//second antenna
	//first module --> 'left'
	//extractionIndices[1,0,0] = 5; //upper
	//extractionIndices[1,0,1] = 6; //lower
	//second antenna
	///second module --> 'right'
	//extractionIndices[1,1,0] = 7; //upper
	//extractionIndices[1,1,1] = 8; //lower
	//third antenna
	//first module --> 'left'
	//extractionIndices[2,0,0] = 9; //upper
	//extractionIndices[2,0,1] = 10; //lower
	//third antenna
	//second module --> 'right'
	//extractionIndices[2,1,0] = 11; //upper
	//extractionIndices[2,1,1] = 12; //lower

	int extractionIndex = extractionIndices[antennaId - 1] [moduleId - 1] [matching_element - 1];
	char *object_name = "GICHCAPA";
	status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);

	if (status != 0) {
		ic_antennas_throwsIdamError2(status, "ic_antennas_module_matching_element_capacity","GICHCAPA", shotNumber, antennaId, moduleId, matching_element);
	}
	return status;
}

int ic_antennas_module_matching_element_capacity_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int matching_element = nodeIndices[2];
	int status = -1;

	//first index = antennaId
	//second index = moduleId
	//third index = matching_element
	int extractionIndices[3][2][2] = {1,2,3,4,5,6,7,8,9,10,11,12};
	//first antenna
	//first module --> 'left'
	//extractionIndices[0,0,0] = 1; //upper
	//extractionIndices[0,0,1] = 2; //lower
	//first antenna
	//second module --> 'right'
	//extractionIndices[0,1,0] = 3; //upper
	//extractionIndices[0,1,1] = 4; //lower
	//second antenna
	//first module --> 'left'
	//extractionIndices[1,0,0] = 5; //upper
	//extractionIndices[1,0,1] = 6; //lower
	//second antenna
	///second module --> 'right'
	//extractionIndices[1,1,0] = 7; //upper
	//extractionIndices[1,1,1] = 8; //lower
	//third antenna
	//first module --> 'left'
	//extractionIndices[2,0,0] = 9; //upper
	//extractionIndices[2,0,1] = 10; //lower
	//third antenna
	//second module --> 'right'
	//extractionIndices[2,1,0] = 11; //upper
	//extractionIndices[2,1,1] = 12; //lower
	//int *ptr_extractionIndices = extractionIndices[0,0,0];
	int extractionIndex = extractionIndices[antennaId - 1][moduleId - 1][matching_element - 1];
	char *object_name = "GICHCAPA";
	status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);

	if (status != 0) {
		ic_antennas_throwsIdamError2(status, "ic_antennas_module_matching_element_capacity_time","GICHCAPA", shotNumber, antennaId, moduleId, matching_element);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_module_pressure(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1.0; //conversion factor
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;

	//first index = antennaId
	//second index = moduleId
	int extractionIndices[3][2] = {1,2,3,4,5,6};

	//first antenna
	//first module --> 'left'
	//extractionIndices[0,0] = 1;
	//first antenna
	//second module --> 'right'
	//extractionIndices[0,1] = 2;
	//second antenna
	//first module --> 'left'
	//extractionIndices[1,0] = 3; //upper
	//second antenna
	///second module --> 'right'
	//extractionIndices[1,1] = 4;
	//third antenna
	//first module --> 'left'
	//extractionIndices[2,0] = 5;
	//third antenna
	//second module --> 'right'
	//extractionIndices[2,1] = 6;
	int extractionIndex = extractionIndices[antennaId - 1][moduleId - 1];
	char *object_name = "GICHVTRANSFO";
	status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);

	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_pressure","GICHVTRANSFO", shotNumber, antennaId, moduleId);
	}
	return status;
}

int ic_antennas_module_pressure_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int status = -1;

	//first index = antennaId
	//second index = moduleId
	int extractionIndices[3][2] = {1,2,3,4,5,6};

	//first antenna
	//first module --> 'left'
	//extractionIndices[0,0] = 1;
	//first antenna
	//second module --> 'right'
	//extractionIndices[0,1] = 2;
	//second antenna
	//first module --> 'left'
	//extractionIndices[1,0] = 3; //upper
	//second antenna
	///second module --> 'right'
	//extractionIndices[1,1] = 4;
	//third antenna
	//first module --> 'left'
	//extractionIndices[2,0] = 5;
	//third antenna
	//second module --> 'right'
	//extractionIndices[2,1] = 6;
	int extractionIndex = extractionIndices[antennaId - 1][moduleId - 1];
	char *object_name = "GICHVTRANSFO";
	status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);

	if (status != 0) {
		ic_antennas_throwsIdamError5(status, "ic_antennas_module_pressure_time","GICHVTRANSFO", shotNumber, antennaId, moduleId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_power_launched(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float f = 1.e3; //conversion factor
	int antennaId = nodeIndices[0];
	int status = -1;

	int extractionIndex = 1;
	if (antennaId == 1) {
		char *object_name = "SICHPQ1";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	} else if (antennaId == 2) {
		char *object_name = "SICHPQ2";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	} else if (antennaId == 3) {
		char *object_name = "SICHPQ4";
		status = setUDABlockSignalFromArcade(object_name, shotNumber, extractionIndex, data_block, nodeIndices, f);
	}

	if (status != 0) {
		ic_antennas_throwsIdamError1(status, "ic_antennas_power","SICHPQ1/SICHPQ2/SICHPQ4", shotNumber, antennaId);
	}
	return status;
}

int ic_antennas_power_launched_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	float *time = NULL;
	float *data = NULL;
	int len;
	int antennaId = nodeIndices[0];
	int status = -1;

	int extractionIndex = 1;
	if (antennaId == 1) {
		char *object_name = "SICHPQ1";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	} else if (antennaId == 2) {
		char *object_name = "SICHPQ2";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	} else if (antennaId == 3) {
		char *object_name = "SICHPQ4";
		status = time_field(object_name, shotNumber, extractionIndex, &time, &data, &len);
	}

	if (status != 0) {
		ic_antennas_throwsIdamError1(status, "ic_antennas_power_launched_time","SICHPQ1/SICHPQ2/SICHPQ4", shotNumber, antennaId);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_frequency(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	int antennaId = nodeIndices[0];
	int val_nb = 1;
	int nb_val = 0;
	char* value = NULL;
	//Reading the antennas frequencies (3 scalars for Q1, Q2 and Q4)
	int status = readStaticParameters(&value, &nb_val, shotNumber, "DFCI", "PILOTAGE", "ICHFREQ", val_nb);
	if (status != 0) {
		ic_antennas_throwsIdamError1(status, "ic_antennas_frequency","DFCI:PILOTAGE:ICHFREQ", shotNumber, antennaId);
		free(value);
	}
	float* pt_float = (float*)value;

	//Getting the time vector
	int rang[2] = {0,0};
	int len;
	float* time = NULL;
	float* data = NULL;
	int status2 = readSignal("GICHANTPOWQ1", shotNumber, 0, rang, &time, &data, &len);

	if (status2 != 0) {
		ic_antennas_throwsIdamError1(status, "ic_antennas_frequency","GICHANTPOWQ1%1", shotNumber, antennaId);
		free(time);
		free(data);
	}
	//Building the FLT_1D antenna/frequency field
	float* frequencies = malloc(sizeof(float)*len);
	int i = 0;
	float f = 1.e6; //conversion factor from MHz to Hz
	for (i = 0; i < len; i++) {
		frequencies[i] = f*pt_float[antennaId - 1]; //vector filled with the same value (frequency is constant in time)
	}

	SetDynamicData(data_block, len, time, frequencies);
	free(pt_float);
	free(data);
	return 0;
}

int ic_antennas_frequency_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	int antennaId = nodeIndices[0];
	float *time = NULL;
	float *data = NULL;
	int len;
	char* object_name = "GICHANTPOWQ1";
	int status = time_field(object_name, shotNumber, 1, &time, &data, &len);
	if (status != 0) {
		ic_antennas_throwsIdamError1(status, "ic_antennas_frequency_time","GICHANTPOWQ1%1", shotNumber, antennaId);
		free(time);
		free(data);
	}
	SetDynamicDataTime(data_block, len, time, data);
	return status;
}

int ic_antennas_modules_strap_outline_r(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int strapId = nodeIndices[2];

	int val_nb = 3;
	int nb_val;
	char* value = NULL;
	//Reading the antennas radial position (3 scalars for Q1, Q2 and Q4)
	int status = readStaticParameters(&value, &nb_val, shotNumber, "EXP-T-S", "Position", "PosICRH", val_nb);
	if (status != 0) {
		ic_antennas_throwsIdamError1(status, "ic_antennas_modules_strap_outline_r","EXP-T-S:Position:PosICRH", shotNumber, antennaId);
		free(value);
	}
	float* pt_float = (float*)value;
	UDA_LOG(UDA_LOG_DEBUG, "After readStaticParameters execution\n");

	//Building the FLT_1D r field
	const int N = 4; //number of radial positions of a strap (2 straps for the left module, 2 straps for the right module)
	float* r = malloc(sizeof(float)*N);
	int i;
	for (i = 0; i < N; i++) {
		r[i] = pt_float[antennaId - 1]; //same radial positions for the 4 points of the outline
	}
	SetStatic1DData(data_block, N, r);
	free(pt_float);
	return 0;
}

int ic_antennas_modules_strap_outline_phi(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {

	int antennaId = nodeIndices[0];
	int moduleId = nodeIndices[1];
	int strapId = nodeIndices[2];

	/*Q1 :
	ic_antennas%antenna(1)%module(1)%strap(1)%outline%phi = 35°
	ic_antennas%antenna(1)%module(1)%strap(2)%outline%phi = 35°
	ic_antennas%antenna(1)%module(2)%strap(1)%outline%phi = 45°
	ic_antennas%antenna(1)%module(2)%strap(2)%outline%phi = 45°

	Q2 :
	ic_antennas%antenna(2)%module(1)%strap(1)%outline%phi = 95°
	ic_antennas%antenna(2)%module(1)%strap(2)%outline%phi = 95°
	ic_antennas%antenna(2)%module(2)%strap(1)%outline%phi = 105°
	ic_antennas%antenna(2)%module(2)%strap(2)%outline%phi = 105°

	Q4:
	ic_antennas%antenna(3)%module(1)%strap(1)%outline%phi = 195°
	ic_antennas%antenna(3)%module(1)%strap(2)%outline%phi = 195°
	ic_antennas%antenna(3)%module(2)%strap(1)%outline%phi = 205°
	ic_antennas%antenna(3)%module(2)%strap(2)%outline%phi = 205°*/
	float f = 3.1415926535897932384626433832795/180.;
	float phi[3][2][2] = {f*35, f*35, f*45, f*45, f*95, f*95, f*105, f*105, f*195, f*195, f*205, f*205};

	/*phi[0][0][0] = f*35; //radians
	phi[0][0][1] = f*35;
	phi[0][1][0] = f*45;
	phi[0][1][1] = f*45;

	phi[1][0][0] = f*95; //radians
	phi[1][0][1] = f*95;
	phi[1][1][0] = f*105;
	phi[1][1][1] = f*105;

	phi[2][0][0] = f*195; //radians
	phi[2][0][1] = f*195;
	phi[2][1][0] = f*205;
	phi[2][1][1] = f*205;*/

	//Building the FLT_1D phi field
	const int N = 4; //number of radial positions of a strap (2 straps for the left module, 2 straps for the right module)
	float* p = malloc(sizeof(float)*N);
	int i = 0;
	for (i = 0; i < N; i++) {
		p[i] = phi[antennaId - 1][moduleId - 1][strapId - 1]; //same radial positions for the 4 points of the outline
	}
	SetStatic1DData(data_block, N, p);
	return 0;
}

int ic_antennas_modules_strap_width_tor(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float width_tor = 0.14;
	setReturnDataFloatScalar(data_block, width_tor, NULL);
	return 0;
}

int ic_antennas_modules_strap_distance_to_conductor(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	float distance_to_conductor = 0.14;
	setReturnDataFloatScalar(data_block, distance_to_conductor, NULL);
	return 0;
}

int ic_antennas_ids_properties_comment(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices) {
	const char* comment = "The value of the field antenna(:)%module(:)%strap(:)%distance_to_conductor is approximately the distance between the front face of the strap and the antenna box. This distance is approximate since it is NOT constant according to the poloidal position.\n The value of antenna(:)%module(:)%strap(:)%outline%phi is currently NOT corrected from the radial position of the strap corners.";
	//on garde les valeurs de phi approchées données plus haut pour les 4 coins du strap
	setReturnDataString(data_block, comment, NULL);
	return 0;
}



