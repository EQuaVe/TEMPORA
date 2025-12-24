/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/gta_system/synchronization.hpp"
#include "mitl2gta/utils/string_construction.hpp"

namespace mitl2gta {

namespace gta {

/*
 * Format accepted by TChecker:
 * sync:<atomic_1>:<atomic_2>:...:<atomic_n>
 *
 * Atomic:
 * proc_name@event
 */

namespace {
std::string declaration(mitl2gta::gta::atomic_sync_constraint_t const &atomic) {
  return atomic.proc_name + "@" + atomic.event;
}
} // namespace

std::string
declaration(mitl2gta::gta::sync_constraints const &sync_constraints) {
  if (sync_constraints.empty()) {
    return "";
  }

  std::vector<std::string> substrings = {"sync"};

  for (mitl2gta::gta::atomic_sync_constraint_t const &atomic :
       sync_constraints) {
    substrings.emplace_back(declaration(atomic));
  }

  return mitl2gta::join(substrings.cbegin(), substrings.cend(), ":");
}

} // namespace gta
} // namespace mitl2gta
