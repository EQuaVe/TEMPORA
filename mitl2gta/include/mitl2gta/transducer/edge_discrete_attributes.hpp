/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/compile/compiler.hpp"
#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/memory/memory_mapping.hpp"
#include "mitl2gta/transducer/edge.hpp"

namespace mitl2gta {

namespace transducer {

std::string provided_attr(
    mitl2gta::transducer::edge_t const &e,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &mapping);

std::string do_attr(
    mitl2gta::transducer::edge_t const &e,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &mapping);

} // namespace transducer

} // namespace mitl2gta
