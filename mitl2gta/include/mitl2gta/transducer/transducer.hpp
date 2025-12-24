/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <vector>

#include "mitl2gta/sharer_automaton/sharer_registry.hpp"
#include "mitl2gta/transducer/edge.hpp"
#include "mitl2gta/transducer/state.hpp"

namespace mitl2gta {

namespace transducer {

struct transducer_t {
  std::vector<mitl2gta::transducer::state_t> states;
  std::vector<mitl2gta::transducer::edge_t> edges;
};

struct transducer_construction_tools_t {
  mitl2gta::transducer::state_id_allocator_t &state_id_allocator;
  mitl2gta::memory::memory_handler_t &mem_handler;
  mitl2gta::clock::clock_registry_t &clk_registry;
  mitl2gta::sharer::sharer_registry_t &sharer_registry;
};

} // namespace transducer

} // namespace mitl2gta
