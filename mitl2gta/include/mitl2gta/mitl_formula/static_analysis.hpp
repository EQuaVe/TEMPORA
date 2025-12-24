/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>
#include <memory>

#include "mitl2gta/mitl_formula/parsed_formula.hpp"

namespace mitl2gta {

namespace mitl {

enum subformula_placement_t {
  TOP_LEVEL,
  NESTED,
  TOP_AND_NESTED,
};

using subformula_placement_map_t =
    std::map<mitl2gta::mitl::formula_t const *,
             mitl2gta::mitl::subformula_placement_t>;

mitl2gta::mitl::subformula_placement_map_t compute_node_placements(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr);

} // namespace mitl

} // namespace mitl2gta
