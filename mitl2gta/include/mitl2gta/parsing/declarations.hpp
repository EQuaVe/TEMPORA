/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <memory>

#include "mitl2gta/mitl_formula/parsed_formula.hpp"

namespace mitl2gta {

namespace mitl {

std::shared_ptr<mitl2gta::mitl::formula_t const>
parse_formula(std::string const &formula);

} // namespace mitl

} // namespace mitl2gta
