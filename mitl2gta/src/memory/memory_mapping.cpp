/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <variant>

#include "mitl2gta/memory/memory_mapping.hpp"
#include "mitl2gta/transducer/static_analysis.hpp"

namespace mitl2gta {

namespace memory {

namespace {

std::string const LOCAL_VARSNAME = "locals";

using live_range_t = std::pair<std::size_t, std::size_t>;
using optional_live_range_t = std::optional<live_range_t>;
using live_range_map_t =
    std::map<mitl2gta::memory::memory_id_t, optional_live_range_t>;

live_range_map_t local_vars_live_ranges(
    mitl2gta::transducer::system_t &system,
    std::set<mitl2gta::memory::memory_id_t> const &local_mems,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::memory::memory_id_t> const &placeholder_memory) {

  live_range_map_t live_ranges;
  for (auto const &m : local_mems) {
    live_ranges.insert({m, std::nullopt});
  }

  auto const &transducers = system.transducers();

  for (std::size_t i = 0; i < transducers.size(); i++) {
    std::set<mitl2gta::memory::memory_id_t> active_mem =
        mitl2gta::transducer::active_memory(
            transducers[i], node_to_truth_value_predictor, placeholder_memory);
    for (mitl2gta::memory::memory_id_t m : active_mem) {
      if (local_mems.find(m) == local_mems.end()) {
        continue;
      }

      optional_live_range_t &live_range_or_none = live_ranges.at(m);
      if (live_range_or_none.has_value()) {
        live_range_or_none->second = i;
      } else {
        live_range_or_none = std::make_pair(i, i + 1);
      }
    }
  }

  return live_ranges;
}

class physical_memory_t {
public:
  physical_memory_t(std::size_t max_size)
      : phy_mem_status(max_size, false), _size(0) {}

  std::size_t allocate() {
    for (std::size_t i = 0; i < phy_mem_status.size(); i++) {
      if (phy_mem_status[i] == false) {
        _size = std::max(_size, i + 1);
        phy_mem_status[i] = true;
        return i;
      }
    }
    throw std::runtime_error("No free memory");
  }

  void deallocate(std::size_t const index) {
    if (phy_mem_status[index] == false) {
      throw std::runtime_error("Attempting to deallocate free memory");
    }
    phy_mem_status[index] = false;
  }

  std::size_t size() const { return _size; }

private:
  std::vector<bool> phy_mem_status;
  std::size_t _size;
};

void add_memory_free_instructions(mitl2gta::transducer::transducer_t &t,
                                  memory_id_t const id, int const null_value) {
  for (auto &e : t.edges) {
    if (std::holds_alternative<mitl2gta::transducer::on_epsilon_t>(e.on()) ||
        std::holds_alternative<mitl2gta::transducer::on_epsilon_node_values_t>(
            e.on())) {
      continue;
    }
    e.actions().insert(
        e.actions().begin(),
        mitl2gta::transducer::set_memory_value_t{id, null_value});
  }
}
} // namespace

memory_mapping_t::memory_mapping_t(
    mitl2gta::memory::mapping_mode_t const mode,
    mitl2gta::transducer::system_t &system,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::memory::memory_id_t> const &placeholder_memory) {
  std::size_t const allocated_memory =
      system.memory_handler().allocated_memory();

  std::set<mitl2gta::memory::memory_id_t> local_mems;
  mitl2gta::boundval_t local_lower =
      std::numeric_limits<mitl2gta::boundval_t>::max();
  mitl2gta::boundval_t local_upper =
      std::numeric_limits<mitl2gta::boundval_t>::min();

  for (std::size_t id = 0; id < allocated_memory; id++) {
    mitl2gta::memory::memory_requirements_t const &req =
        system.memory_handler().memory_requirements(id);

    switch (req.type) {
    case mitl2gta::memory::memory_type_t::PERSISTENT: {
      std::string name = "global" + std::to_string(id);
      mitl2gta::gta::array_integer_variables_t global_var(name, 1, req.range,
                                                          req.initial_value);
      _int_vars.push_back(global_var);
      _mem_to_var.insert({id, mitl2gta::gta::varname(global_var, 0)});
      break;
    }

    case mitl2gta::memory::memory_type_t::LOCAL_TO_SINGLE_ROUND: {
      local_mems.insert(id);
      local_lower = std::min(local_lower, req.range.lower_bound_value());
      local_upper = std::max(local_lower, req.range.upper_bound_value());
      break;
    }
    }
  }

  switch (mode) {
  case mitl2gta::memory::mapping_mode_t::ONE_TO_ONE: {
    one_to_one_mapping(local_mems, local_lower, local_upper);
    break;
  }

  case mitl2gta::memory::mapping_mode_t::OPTIMISED: {
    optimised_mapping(system, local_mems, local_lower, local_upper,
                      node_to_truth_value_predictor, placeholder_memory);
    break;
  }
  }
}

void memory_mapping_t::one_to_one_mapping(
    std::set<mitl2gta::memory::memory_id_t> const &local_mems,
    mitl2gta::boundval_t const local_lower,
    mitl2gta::boundval_t const local_upper) {
  if (local_mems.size() == 0) {
    return;
  }

  mitl2gta::gta::array_integer_variables_t local_vars(
      LOCAL_VARSNAME, local_mems.size(),
      mitl2gta::gta::integer_variable_range_t(local_lower, local_upper),
      local_lower);
  _int_vars.push_back(local_vars);

  std::size_t curr_index = 0;
  for (auto const id : local_mems) {
    _mem_to_var.insert({id, mitl2gta::gta::varname(local_vars, curr_index)});
    curr_index += 1;
  }
}

void memory_mapping_t::optimised_mapping(
    mitl2gta::transducer::system_t &system,
    std::set<mitl2gta::memory::memory_id_t> const &local_mems,
    mitl2gta::boundval_t const local_lower,
    mitl2gta::boundval_t const local_upper,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::memory::memory_id_t> const &placeholder_memory) {

  if (local_mems.size() == 0) {
    return;
  }

  std::size_t const num_transducers = system.transducers().size();

  live_range_map_t live_ranges = local_vars_live_ranges(
      system, local_mems, node_to_truth_value_predictor, placeholder_memory);

  std::vector<std::set<mitl2gta::memory::memory_id_t>> live_begin(
      num_transducers);

  std::vector<std::set<mitl2gta::memory::memory_id_t>> live_end(
      num_transducers);

  for (auto const &[id, range] : live_ranges) {
    if (!range.has_value()) {
      continue;
    }

    live_begin.at(range->first).insert(id);
    live_end.at(range->second).insert(id);
  }

  physical_memory_t physical_memory(local_mems.size());
  std::map<mitl2gta::memory::memory_id_t, std::size_t> memory_to_index;

  for (std::size_t i = 0; i < num_transducers; i++) {
    for (mitl2gta::memory::memory_id_t freed_mem : live_end[i]) {
      physical_memory.deallocate(memory_to_index.at(freed_mem));
      add_memory_free_instructions(system.transducers()[i], freed_mem,
                                   local_lower);
    }

    for (mitl2gta::memory::memory_id_t new_mem : live_begin[i]) {
      std::size_t allocated_index = physical_memory.allocate();
      memory_to_index.insert({new_mem, allocated_index});
    }
  }

  mitl2gta::gta::array_integer_variables_t local_vars(
      LOCAL_VARSNAME, physical_memory.size(),
      mitl2gta::gta::integer_variable_range_t(local_lower, local_upper),
      local_lower);
  _int_vars.push_back(local_vars);

  for (auto const id : local_mems) {
    if (!live_ranges.at(id).has_value()) {
      continue;
    }

    std::string const varname =
        mitl2gta::gta::varname(local_vars, memory_to_index.at(id));
    _mem_to_var.insert({id, varname});
  }
}

std::string const &
memory_mapping_t::mapped_var(mitl2gta::memory::memory_id_t const id) const {
  return _mem_to_var.at(id);
}

} // namespace memory

} // namespace mitl2gta
