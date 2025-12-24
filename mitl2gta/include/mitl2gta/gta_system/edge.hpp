/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>

#include "mitl2gta/utils/string_construction.hpp"

namespace mitl2gta {
namespace gta {

struct edge_t {
  std::string from_loc_name;
  std::string to_loc_name;
  std::string event;
  mitl2gta::gta::attributes_t attributes;
};

std::string declaration(mitl2gta::gta::edge_t const &e,
                        std::string const &proc_name);
} // namespace gta
} // namespace mitl2gta
