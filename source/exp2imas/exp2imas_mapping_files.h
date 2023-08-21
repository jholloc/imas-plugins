#ifndef IMAS_PLUGINS_EXP2IMAS_MAPPING_FILES_H
#define IMAS_PLUGINS_EXP2IMAS_MAPPING_FILES_H

#include <string>

namespace uda {
namespace exp2imas {

std::string get_mapping_filename(const std::string& ids_version, const std::string& element);
std::string get_machine_mapping_filename(const std::string& experiment, const std::string& element, int shot);

} // namespace exp2imas
} // namespace uda

#endif // IMAS_PLUGINS_EXP2IMAS_MAPPING_FILES_H
