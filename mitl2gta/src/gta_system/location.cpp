/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <array>

#include "mitl2gta/gta_system/location.hpp"

namespace mitl2gta {

namespace gta {

/*
 * Format accepted by TChecker:
 * location:proc_name:loc_name{attributes}
 */
std::string declaration(mitl2gta::gta::location_t const &loc,
                        std::string const &proc_name) {

  std::array<std::string, 3> const substrings{"location", proc_name, loc.name};

  return mitl2gta::join(substrings.cbegin(), substrings.cend(), ":") +
         mitl2gta::gta::attributes_to_string(loc.attributes);
}
} // namespace gta

} // namespace mitl2gta
