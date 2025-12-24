/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <optional>
#include <variant>

#include "mitl2gta/clock/clock_registry.hpp"
#include "mitl2gta/compile/id_allocation.hpp"
#include "mitl2gta/memory/memory_handler.hpp"

namespace mitl2gta {

namespace sharer {

enum p_until_q_sharer_type_t {
  UNTIMED = 0,
  TIMED_FIRST_WITNESS = 1,
  TIMED_FIRST_AND_LAST_WITNESS = 2,
};

static_assert(TIMED_FIRST_AND_LAST_WITNESS > TIMED_FIRST_WITNESS,
              "Stronger type subsumes weaker type");
static_assert(TIMED_FIRST_WITNESS > UNTIMED,
              "Stronger type subsumes weaker type");

struct time_from_first_event_t {
  mitl2gta::clock::clock_id_t maintaining_clk;
};

struct next_event_t {
  mitl2gta::clock::clock_id_t predicting_clk;
};

struct next_q_event_t {
  mitl2gta::compilation::node_id_t q_id;
  mitl2gta::clock::clock_id_t predicting_clk;
};

int const SHARER_FALSE_VAL = 0;
int const SHARER_TRUE_VAL = 1;

struct p_until_q_sharer_t {
  mitl2gta::compilation::node_id_t p_id;
  mitl2gta::compilation::node_id_t q_id;
  mitl2gta::sharer::p_until_q_sharer_type_t type;

  mitl2gta::memory::memory_id_t p_until_q_truth_value;
  mitl2gta::memory::memory_id_t next_p_until_q_truth_value;

  std::optional<mitl2gta::clock::clock_id_t> first_witness_predicting_clk;
  std::optional<mitl2gta::clock::clock_id_t> last_witness_predicting_clk;
};

using sharer_automaton_t = std::variant<
    mitl2gta::sharer::time_from_first_event_t, mitl2gta::sharer::next_event_t,
    mitl2gta::sharer::next_q_event_t, mitl2gta::sharer::p_until_q_sharer_t>;

} // namespace sharer

} // namespace mitl2gta
