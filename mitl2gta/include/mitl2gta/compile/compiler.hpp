/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/compile/compiled_node_registry.hpp"
#include "mitl2gta/gta_system/system.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"
#include "mitl2gta/sharer_automaton/translation.hpp"
#include "mitl2gta/transducer/system.hpp"

namespace mitl2gta {

namespace compilation {

// TODO: Substitute this in all places
using placeholder_memory_map_t =
    std::map<mitl2gta::compilation::node_id_t, mitl2gta::memory::memory_id_t>;

class compiler_t {
public:
  compiler_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr,
      mitl2gta::compilation::verification_mode_t const verification_mode);

  mitl2gta::gta::system_t translate_to_gta_system(
      mitl2gta::sharer::sharer_mode_t const mode,
      mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
      mitl2gta::compilation::translation_specs_t const &specs) const;

private:
  void translate_to_transducer_system(
      mitl2gta::transducer::system_t &system,
      mitl2gta::sharer::sharer_mode_t const mode,
      mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
      mitl2gta::compilation::translation_specs_t const &specs,
      mitl2gta::compilation::node_to_truth_value_predictor_t &mapping_to_fill)
      const;

  mitl2gta::compilation::placeholder_memory_map_t allocate_placeholder_memory(
      mitl2gta::compilation::node_to_truth_value_predictor_t &mapping,
      mitl2gta::memory::memory_handler_t &handler) const;

  std::vector<mitl2gta::compilation::node_id_t> topological_sort() const;

  mitl2gta::compilation::verification_mode_t const _verification_mode;

  mitl2gta::compilation::node_id_t _root_id;

  mitl2gta::compilation::compiled_node_registry_t _nodes_registry;
};

} // namespace compilation
} // namespace mitl2gta
