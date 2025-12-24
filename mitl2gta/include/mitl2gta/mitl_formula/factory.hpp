/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/mitl_formula/parsed_formula.hpp"

namespace mitl2gta {
namespace mitl {

enum parsing_mode_t {
  SUBFORMULA_SHARING_ENABLED,
  SUBFORMULA_SHARING_DISABLED,
};

std::shared_ptr<mitl2gta::mitl::formula_t const>
construct_formula_graph(std::string const &formula,
                        mitl2gta::mitl::parsing_mode_t const parsing_mode);

} // namespace mitl
} // namespace mitl2gta
