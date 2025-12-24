/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/sharer_automaton/sharer_registry.hpp"
#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/sharer_automaton/sharer_types.hpp"

namespace mitl2gta {
namespace sharer {

sharer_registry_t::sharer_registry_t(mitl2gta::sharer::sharer_mode_t const mode)
    : _mode(mode) {}

mitl2gta::sharer::time_from_first_event_t
sharer_registry_t::create_time_from_first_event_t(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::clock::clock_registry_t &reg) {
  mitl2gta::clock::clock_id_t const maintaining_clk =
      reg.register_clock(mitl2gta::gta::gta_clock_type_t::HISTORY_INF);
  mitl2gta::sharer::time_from_first_event_t const automaton{maintaining_clk};

  _dependencies[id].emplace_back(automaton);
  return automaton;
}

mitl2gta::sharer::time_from_first_event_t
sharer_registry_t::request_time_from_first_event(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::clock::clock_registry_t &clk_reg,
    mitl2gta::memory::memory_handler_t &mem_handler) {

  if (_mode == mitl2gta::sharer::sharer_mode_t::SHARING_DISABLED) {
    return create_time_from_first_event_t(id, clk_reg);
  }

  for (auto const &it : _dependencies) {
    for (auto const &automaton : it.second) {
      if (std::holds_alternative<mitl2gta::sharer::time_from_first_event_t>(
              automaton)) {
        return std::get<mitl2gta::sharer::time_from_first_event_t>(automaton);
      }
    }
  }

  return create_time_from_first_event_t(id, clk_reg);
}

mitl2gta::sharer::next_event_t
sharer_registry_t::create_next_event(mitl2gta::compilation::node_id_t const id,
                                     mitl2gta::clock::clock_registry_t &reg) {
  mitl2gta::clock::clock_id_t const pred_clk =
      reg.register_clock(mitl2gta::gta::gta_clock_type_t::PROPHECY);
  mitl2gta::sharer::next_event_t const automaton{pred_clk};

  _dependencies[id].emplace_back(automaton);
  return automaton;
}

mitl2gta::sharer::next_event_t sharer_registry_t::request_next_event(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::clock::clock_registry_t &clk_reg,
    mitl2gta::memory::memory_handler_t &mem_handler) {

  if (_mode == mitl2gta::sharer::sharer_mode_t::SHARING_DISABLED) {
    return create_next_event(id, clk_reg);
  }

  for (auto const &it : _dependencies) {
    for (auto const &automaton : it.second) {
      if (std::holds_alternative<mitl2gta::sharer::next_event_t>(automaton)) {
        return std::get<mitl2gta::sharer::next_event_t>(automaton);
      }
    }
  }

  return create_next_event(id, clk_reg);
}

mitl2gta::sharer::next_q_event_t sharer_registry_t::create_next_q_event(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::compilation::node_id_t const q_id,
    mitl2gta::clock::clock_registry_t &reg) {

  mitl2gta::clock::clock_id_t const pred_clk =
      reg.register_clock(mitl2gta::gta::gta_clock_type_t::PROPHECY);
  mitl2gta::sharer::next_q_event_t const automaton{q_id, pred_clk};

  _dependencies[id].emplace_back(automaton);
  return automaton;
}

mitl2gta::sharer::next_q_event_t sharer_registry_t::request_next_q_event(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::compilation::node_id_t const q_id,
    mitl2gta::clock::clock_registry_t &clk_reg,
    mitl2gta::memory::memory_handler_t &mem_handler) {

  if (_mode == mitl2gta::sharer::sharer_mode_t::SHARING_DISABLED) {
    return create_next_q_event(id, q_id, clk_reg);
  }

  for (auto const &it : _dependencies) {
    for (auto const &automaton : it.second) {
      if (std::holds_alternative<mitl2gta::sharer::next_q_event_t>(automaton)) {
        auto &a = std::get<mitl2gta::sharer::next_q_event_t>(automaton);
        if (a.q_id == q_id) {
          return a;
        }
      }
    }
  }

  return create_next_q_event(id, q_id, clk_reg);
}

mitl2gta::sharer::p_until_q_sharer_t sharer_registry_t::create_p_until_q_sharer(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::compilation::node_id_t const p_id,
    mitl2gta::compilation::node_id_t const q_id,
    mitl2gta::sharer::p_until_q_sharer_type_t const type,
    mitl2gta::clock::clock_registry_t &reg,
    mitl2gta::memory::memory_handler_t &mem_handler) {

  mitl2gta::gta::integer_variable_range_t mem_range(
      std::min(mitl2gta::sharer::SHARER_FALSE_VAL,
               mitl2gta::sharer::SHARER_TRUE_VAL),
      std::max(mitl2gta::sharer::SHARER_FALSE_VAL,
               mitl2gta::sharer::SHARER_TRUE_VAL));

  mitl2gta::memory::memory_requirements_t mem_reqs{
      mem_range, mitl2gta::sharer::SHARER_FALSE_VAL,
      mitl2gta::memory::memory_type_t::LOCAL_TO_SINGLE_ROUND};

  mitl2gta::memory::memory_id_t const p_until_q_val =
      mem_handler.request_memory(mem_reqs);
  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      mem_handler.request_memory(mem_reqs);

  std::optional<mitl2gta::clock::clock_id_t> first_witness(std::nullopt);
  std::optional<mitl2gta::clock::clock_id_t> last_witness(std::nullopt);

  if (type >= mitl2gta::sharer::p_until_q_sharer_type_t::TIMED_FIRST_WITNESS) {
    mitl2gta::sharer::next_q_event_t next_q_event =
        request_next_q_event(id, q_id, reg, mem_handler);
    first_witness = next_q_event.predicting_clk;
  }

  if (type >=
      mitl2gta::sharer::p_until_q_sharer_type_t::TIMED_FIRST_AND_LAST_WITNESS) {
    last_witness =
        reg.register_clock(mitl2gta::gta::gta_clock_type_t::PROPHECY);
  }

  mitl2gta::sharer::p_until_q_sharer_t automaton{
      p_id,          q_id,        type, p_until_q_val, next_p_until_q_val,
      first_witness, last_witness};

  _dependencies[id].emplace_front(automaton);
  return automaton;
}

mitl2gta::sharer::p_until_q_sharer_t
sharer_registry_t::request_p_until_q_sharer(
    mitl2gta::compilation::node_id_t const id,
    mitl2gta::compilation::node_id_t const p_id,
    mitl2gta::compilation::node_id_t const q_id,
    enum mitl2gta::sharer::p_until_q_sharer_type_t const type,
    mitl2gta::clock::clock_registry_t &clk_reg,
    mitl2gta::memory::memory_handler_t &mem_handler) {
  if (_mode == mitl2gta::sharer::sharer_mode_t::SHARING_DISABLED) {
    return create_p_until_q_sharer(id, p_id, q_id, type, clk_reg, mem_handler);
  }

  for (auto &it : _dependencies) {
    for (auto &automaton : it.second) {
      if (!std::holds_alternative<mitl2gta::sharer::p_until_q_sharer_t>(
              automaton)) {
        continue;
      }
      auto &sharer = std::get<mitl2gta::sharer::p_until_q_sharer_t>(automaton);

      if (sharer.p_id != p_id || sharer.q_id != q_id) {
        continue;
      }

      if (sharer.type >= type) {
        return sharer;
      }

      if (type >=
              mitl2gta::sharer::p_until_q_sharer_type_t::TIMED_FIRST_WITNESS &&
          (!sharer.first_witness_predicting_clk.has_value())) {

        mitl2gta::sharer::next_q_event_t next_q_event =
            request_next_q_event(it.first, q_id, clk_reg, mem_handler);
        sharer.first_witness_predicting_clk = next_q_event.predicting_clk;
      }

      if (type >= mitl2gta::sharer::p_until_q_sharer_type_t::
                      TIMED_FIRST_AND_LAST_WITNESS &&
          (!sharer.last_witness_predicting_clk.has_value())) {
        sharer.last_witness_predicting_clk =
            clk_reg.register_clock(mitl2gta::gta::gta_clock_type_t::PROPHECY);
      }

      sharer.type = type;

      return sharer;
    }
  }

  return create_p_until_q_sharer(id, p_id, q_id, type, clk_reg, mem_handler);
}
} // namespace sharer
} // namespace mitl2gta
