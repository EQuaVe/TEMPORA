/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <array>

#include "mitl2gta/gta_system/edge.hpp"

namespace mitl2gta {

namespace gta {

/*
 * Format accepted by TChecker:
 * edge:proc_name:from_loc_name:to_loc_name:event{attributes}
 */
std::string declaration(mitl2gta::gta::edge_t const &e,
                        std::string const &proc_name) {

  std::array<std::string, 5> const substrings{
      "edge", proc_name, e.from_loc_name, e.to_loc_name, e.event};

  return mitl2gta::join(substrings.cbegin(), substrings.cend(), ":") +
         mitl2gta::gta::attributes_to_string(e.attributes);
}
} // namespace gta

} // namespace mitl2gta
