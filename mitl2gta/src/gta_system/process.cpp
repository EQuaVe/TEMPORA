/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <array>

#include "mitl2gta/gta_system/process.hpp"

namespace mitl2gta {

namespace gta {

/*
 * Format accepted by TChecker:
 * process:proc_name
 * location declarations
 * edge declarations
 */
std::string declaration(mitl2gta::gta::process_t const &proc) {

  std::array<std::string, 2> const name_decl_substrings{"process", proc.name};

  std::string name_decl = mitl2gta::join(name_decl_substrings.cbegin(),
                                         name_decl_substrings.cend(), ":");

  std::vector<std::string> loc_decls_substrings;
  for (auto const &loc : proc.locations) {
    loc_decls_substrings.emplace_back(
        mitl2gta::gta::declaration(loc, proc.name));
  }
  std::string loc_decls = mitl2gta::join(loc_decls_substrings.cbegin(),
                                         loc_decls_substrings.cend(), "\n");

  std::vector<std::string> edge_decls_substrings;
  for (auto const &edge : proc.edges) {
    edge_decls_substrings.emplace_back(
        mitl2gta::gta::declaration(edge, proc.name));
  }
  std::string edge_decls = mitl2gta::join(edge_decls_substrings.cbegin(),
                                          edge_decls_substrings.cend(), "\n");

  std::array<std::string, 3> const decl_substrings{name_decl, loc_decls,
                                                   edge_decls};

  return mitl2gta::join(decl_substrings.cbegin(), decl_substrings.cend(),
                        "\n\n");
}
} // namespace gta

} // namespace mitl2gta
