/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>
#include <vector>

#include "mitl2gta/clock/clock_registry.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"

namespace mitl2gta {

namespace transducer {

std::vector<std::string> gta_programs_disjunction(
    std::vector<mitl2gta::transducer::gta_program_t> const &prog,
    mitl2gta::clock::clock_registry_t const &reg);

} // namespace transducer

} // namespace mitl2gta
