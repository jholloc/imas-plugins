#ifndef UDA_PLUGIN_IMAS_EXTRACT_INDICES
#define UDA_PLUGIN_IMAS_EXTRACT_INDICES

int extract_array_indices(const char* input, char** output, int** indices);
char* indices_to_string(const int* indices, int num_indices);

#endif // UDA_PLUGIN_IMAS_EXTRACT_INDICES
