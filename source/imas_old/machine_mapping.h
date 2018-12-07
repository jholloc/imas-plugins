#ifndef IMAS_PLUGINS_MACHINE_MAPPING_H
#define IMAS_PLUGINS_MACHINE_MAPPING_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace uda {
namespace imas_mapping {
class MachineMapping;
}
}
typedef uda::imas_mapping::MachineMapping MACHINE_MAPPING;
#else
typedef struct MachineMapping MACHINE_MAPPING;
#endif

MACHINE_MAPPING* new_mapping();
void free_mapping(MACHINE_MAPPING* mapping);
const char* mapping_host(MACHINE_MAPPING* mapping, const char* machine, const char* ids);
const char* mapping_plugin(MACHINE_MAPPING* mapping, const char* machine, const char* ids);

#ifdef __cplusplus
}
#endif


#endif // IMAS_PLUGINS_MACHINE_MAPPING_H
