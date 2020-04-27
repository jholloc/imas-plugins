
#ifndef UDA_PLUGIN_WEST_LH_ANTENNAS_H
#define UDA_PLUGIN_WEST_LH_ANTENNAS_H

#include <clientserver/udaStructs.h>

int lh_antennas_ids_properties_comment(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_power(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_power_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_antenna_power(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_antenna_power_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_power_forward(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_power_forward_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_power_reflected(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_power_reflected_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_reflection_coefficient(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_reflection_coefficient_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_forward(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_forward_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_launched(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_launched_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_reflected(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_power_reflected_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_reflection_coefficient(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_reflection_coefficient_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_phase(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_modules_phase_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_phase_average(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_phase_average_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_n_parallel_peak(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_n_parallel_peak_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_position_r(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_position_r_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_position_z(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_position_definition(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_pressure_tank(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int lh_antennas_pressure_tank_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
void lh_antennas_throwsIdamError(int status, char* methodName, char * objectName, int shotNumber);
void lh_antennas_throwsIdamError2(int status, char* methodName, char * objectName, int shotNumber, int antennaId);

#endif // UDA_PLUGIN_WEST_LH_ANTENNAS_H
