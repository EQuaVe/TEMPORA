/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <deque>
#include <map>

#include "mitl2gta/clock/clock_registry.hpp"
#include "mitl2gta/compile/id_allocation.hpp"
#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/sharer_automaton/sharer_types.hpp"

namespace mitl2gta {

namespace sharer {

enum sharer_mode_t {
  SHARING_ENABLED,
  SHARING_DISABLED,
};

class sharer_registry_t {
public:
  sharer_registry_t(enum mitl2gta::sharer::sharer_mode_t const mode);

  mitl2gta::sharer::time_from_first_event_t request_time_from_first_event(
      mitl2gta::compilation::node_id_t const id,
      mitl2gta::clock::clock_registry_t &clk_reg,
      mitl2gta::memory::memory_handler_t &mem_handler);

  mitl2gta::sharer::next_event_t
  request_next_event(mitl2gta::compilation::node_id_t const id,
                     mitl2gta::clock::clock_registry_t &clk_reg,
                     mitl2gta::memory::memory_handler_t &mem_handler);

  mitl2gta::sharer::next_q_event_t
  request_next_q_event(mitl2gta::compilation::node_id_t const id,
                       mitl2gta::compilation::node_id_t const q_id,
                       mitl2gta::clock::clock_registry_t &clk_reg,
                       mitl2gta::memory::memory_handler_t &mem_handler);

  mitl2gta::sharer::p_until_q_sharer_t request_p_until_q_sharer(
      mitl2gta::compilation::node_id_t const id,
      mitl2gta::compilation::node_id_t const p_id,
      mitl2gta::compilation::node_id_t const q_id,
      enum mitl2gta::sharer::p_until_q_sharer_type_t const type,
      mitl2gta::clock::clock_registry_t &clk_reg,
      mitl2gta::memory::memory_handler_t &mem_handler);

  std::deque<mitl2gta::sharer::sharer_automaton_t> const &
  dependencies(mitl2gta::compilation::node_id_t const id) {
    return _dependencies[id];
  }

private:
  mitl2gta::sharer::time_from_first_event_t
  create_time_from_first_event_t(mitl2gta::compilation::node_id_t const id,
                                 mitl2gta::clock::clock_registry_t &reg);

  mitl2gta::sharer::next_event_t
  create_next_event(mitl2gta::compilation::node_id_t const id,
                    mitl2gta::clock::clock_registry_t &reg);

  mitl2gta::sharer::next_q_event_t
  create_next_q_event(mitl2gta::compilation::node_id_t const id,
                      mitl2gta::compilation::node_id_t const q_id,
                      mitl2gta::clock::clock_registry_t &reg);

  mitl2gta::sharer::p_until_q_sharer_t
  create_p_until_q_sharer(mitl2gta::compilation::node_id_t const id,
                          mitl2gta::compilation::node_id_t const p_id,
                          mitl2gta::compilation::node_id_t const q_id,
                          mitl2gta::sharer::p_until_q_sharer_type_t const type,
                          mitl2gta::clock::clock_registry_t &reg,
                          mitl2gta::memory::memory_handler_t &mem_handler);

  using sharer_automaton_list_t =
      std::deque<mitl2gta::sharer::sharer_automaton_t>;

  std::map<mitl2gta::compilation::node_id_t, sharer_automaton_list_t>
      _dependencies;

  mitl2gta::sharer::sharer_mode_t const _mode;
};

} // namespace sharer

} // namespace mitl2gta
