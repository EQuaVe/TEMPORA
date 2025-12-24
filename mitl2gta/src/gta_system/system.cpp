/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <array>

#include "mitl2gta/gta_system/system.hpp"

namespace mitl2gta {

namespace gta {

/*
 * Format accepted by TChecker:
 * system:sys_name
 * event declarations
 * variable declarations
 * process declarations
 */
std::string declaration(mitl2gta::gta::system_t const &sys) {

  std::array<std::string, 2> const name_decl_substrings{"system", sys.name};

  std::string name_decl = mitl2gta::join(name_decl_substrings.cbegin(),
                                         name_decl_substrings.cend(), ":");

  std::vector<std::string> event_decl_substrings;
  for (auto const &e : sys.events) {
    event_decl_substrings.emplace_back("event:" + e);
  }
  std::string event_decl = mitl2gta::join(event_decl_substrings.cbegin(),
                                          event_decl_substrings.cend(), "\n");

  std::vector<std::string> int_variables_decl_substrings;
  for (auto const &vars : sys.int_variables) {
    int_variables_decl_substrings.emplace_back(
        mitl2gta::gta::declaration(vars));
  }
  std::string int_variables_decl =
      mitl2gta::join(int_variables_decl_substrings.cbegin(),
                     int_variables_decl_substrings.cend(), "\n");

  std::vector<std::string> clock_variables_decl_substrings;
  for (auto const &vars : sys.clock_variables) {
    clock_variables_decl_substrings.emplace_back(
        mitl2gta::gta::declaration(vars));
  }
  std::string clock_variables_decl =
      mitl2gta::join(clock_variables_decl_substrings.cbegin(),
                     clock_variables_decl_substrings.cend(), "\n");

  std::vector<std::string> proc_decls_substrings;
  for (auto const &proc : sys.process) {
    proc_decls_substrings.emplace_back(mitl2gta::gta::declaration(proc));
  }

  std::string proc_decls = mitl2gta::join(proc_decls_substrings.cbegin(),
                                          proc_decls_substrings.cend(), "\n\n");

  std::vector<std::string> sync_decls_substrings;
  for (auto const &sync : sys.sync_constraints) {
    sync_decls_substrings.emplace_back(mitl2gta::gta::declaration(sync));
  }

  std::string sync_decls = mitl2gta::join(sync_decls_substrings.cbegin(),
                                          sync_decls_substrings.cend(), "\n");

  std::array<std::string, 6> const decl_substrings{
      name_decl,  event_decl, int_variables_decl, clock_variables_decl,
      proc_decls, sync_decls};

  return mitl2gta::join(decl_substrings.cbegin(), decl_substrings.cend(),
                        "\n\n");
}

} // namespace gta

} // namespace mitl2gta
