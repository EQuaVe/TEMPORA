/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <set>

#include "mitl2gta/gta_system/static_analysis.hpp"

namespace mitl2gta {

namespace gta {

std::set<std::string> labels(mitl2gta::gta::system_t const &sys) {
  std::set<std::string> res;
  for (auto const &p : sys.process) {
    for (auto const &l : p.locations) {
      mitl2gta::gta::attributes_t const &attrs = l.attributes;
      auto const &labels = attrs.find("labels");
      if (labels != attrs.end()) {
        res.insert(labels->second);
      }
    }
  }

  return res;
}

} // namespace gta
} // namespace mitl2gta
