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

struct location_t {
  std::string name;
  mitl2gta::gta::attributes_t attributes;
};

std::string declaration(mitl2gta::gta::location_t const &loc,
                        std::string const &proc_name);
} // namespace gta

} // namespace mitl2gta
