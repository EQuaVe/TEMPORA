/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/sharer_automaton/sharer_types.hpp"
#include "mitl2gta/transducer/system.hpp"
#include "mitl2gta/transducer/transducer.hpp"

namespace mitl2gta {
namespace sharer {

enum p_until_q_sharer_states_t {
  TWO_STATE,
  THREE_STATE,
};

mitl2gta::transducer::transducer_t to_transducer(
    mitl2gta::sharer::sharer_automaton_t const &automaton,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::transducer::system_for_t const system_for,
    mitl2gta::transducer::transducer_construction_tools_t &tools);

} // namespace sharer
} // namespace mitl2gta
