/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/boolean_expression/expression.hpp"
#include "mitl2gta/memory/memory_mapping.hpp"

namespace mitl2gta {
namespace boolean {

std::string
boolean_expression(mitl2gta::boolean::expression_t const &expr,
                   mitl2gta::memory::memory_mapping_t const &mapping);

} // namespace boolean
} // namespace mitl2gta
