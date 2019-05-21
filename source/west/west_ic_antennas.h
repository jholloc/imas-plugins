
#ifndef IDAM_PLUGIN_WEST_IC_ANTENNAS_H
#define IDAM_PLUGIN_WEST_IC_ANTENNAS_H

#include <clientserver/udaStructs.h>

int ic_antennas_ids_properties_comment(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_power_forward(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_power_forward_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_power_reflected(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_power_reflected_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_phase_forward(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_phase_forward_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_phase_reflected(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_phase_reflected_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_voltage_phase(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_voltage_phase_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_current(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_current_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_voltage(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_voltage_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_matching_element_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_matching_element_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_matching_element_capacity(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_matching_element_capacity_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_pressure(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_module_pressure_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_name(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_identifier(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_power_launched(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_power_launched_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_frequency(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_frequency_time(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_modules_strap_outline_r(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_modules_strap_outline_phi(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_modules_strap_width_tor(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
int ic_antennas_modules_strap_distance_to_conductor(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);

#endif // IDAM_PLUGIN_WEST_IC_ANTENNAS_H
