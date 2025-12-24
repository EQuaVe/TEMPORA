/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>
#include <vector>

#include "mitl2gta/gta_system/edge.hpp"
#include "mitl2gta/gta_system/location.hpp"

namespace mitl2gta {
namespace gta {

struct process_t {
  std::string name;
  std::vector<mitl2gta::gta::location_t> locations;
  std::vector<mitl2gta::gta::edge_t> edges;
};

std::string declaration(mitl2gta::gta::process_t const &proc);

} // namespace gta
} // namespace mitl2gta
