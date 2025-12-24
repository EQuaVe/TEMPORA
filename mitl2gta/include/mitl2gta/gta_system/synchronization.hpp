/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>
#include <vector>
namespace mitl2gta {

namespace gta {

struct atomic_sync_constraint_t {
  std::string proc_name;
  std::string event;
};

using sync_constraints = std::vector<mitl2gta::gta::atomic_sync_constraint_t>;

std::string
declaration(mitl2gta::gta::sync_constraints const &sync_constraints);

} // namespace gta
} // namespace mitl2gta
