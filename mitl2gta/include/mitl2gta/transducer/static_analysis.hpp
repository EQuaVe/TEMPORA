/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <set>

#include "mitl2gta/compile/compiler.hpp"
#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/transducer/transducer.hpp"

namespace mitl2gta {
namespace transducer {

std::set<mitl2gta::memory::memory_id_t> active_memory(
    mitl2gta::transducer::transducer_t const &t,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory);

bool has_epsilon_transition(
    mitl2gta::transducer::transducer_t const &transducer);

} // namespace transducer
} // namespace mitl2gta
