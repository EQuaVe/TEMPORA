/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>
#include <vector>

#include "mitl2gta/gta_system/process.hpp"
#include "mitl2gta/gta_system/synchronization.hpp"
#include "mitl2gta/gta_system/variable.hpp"

namespace mitl2gta {
namespace gta {

struct system_t {
  std::string name;
  std::vector<std::string> events;
  std::vector<std::string> labels;
  std::vector<mitl2gta::gta::array_integer_variables_t> int_variables;
  std::vector<mitl2gta::gta::gta_clock_variable_t> clock_variables;
  std::vector<mitl2gta::gta::process_t> process;
  std::vector<mitl2gta::gta::sync_constraints> sync_constraints;
};

std::string declaration(mitl2gta::gta::system_t const &sys);

} // namespace gta
} // namespace mitl2gta
