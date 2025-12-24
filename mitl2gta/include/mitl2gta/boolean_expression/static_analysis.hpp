/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <set>

#include "mitl2gta/boolean_expression/expression.hpp"

namespace mitl2gta {
namespace boolean {

std::set<mitl2gta::memory::memory_id_t>
ids_in_expr(mitl2gta::boolean::expression_t const &expr);
} // namespace boolean
} // namespace mitl2gta
