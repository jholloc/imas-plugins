#ifndef UDA_PLUGIN_EXP2IMAS_EXP2IMAS_MDS_H
#define UDA_PLUGIN_EXP2IMAS_EXP2IMAS_MDS_H

#ifdef __cplusplus
extern "C" {
#endif

int
mds_get(const char* experiment, const char* signalName, int shot, float** time, float** data, int* len, int* time_len, int time_dim);

#ifdef __cplusplus
}
#endif

#endif // UDA_PLUGIN_EXP2IMAS_EXP2IMAS_MDS_H
