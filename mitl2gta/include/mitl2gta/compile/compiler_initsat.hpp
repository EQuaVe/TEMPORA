/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/gta_system/system.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"
#include "mitl2gta/mitl_formula/static_analysis.hpp"
#include "mitl2gta/sharer_automaton/translation.hpp"
#include "mitl2gta/transducer/system.hpp"

namespace mitl2gta {

namespace compilation {

class compiler_initsat_t final : virtual mitl2gta::mitl::formula_visitor_t {
public:
  compiler_initsat_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr,
      mitl2gta::compilation::verification_mode_t const mode);

  mitl2gta::gta::system_t translate_to_gta_system(
      mitl2gta::sharer::sharer_mode_t const mode,
      mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
      mitl2gta::compilation::translation_specs_t const &specs);

private:
  virtual void visit(mitl2gta::mitl::atomic_proposition_t const &node);
  virtual void visit(mitl2gta::mitl::atomic_constant_t const &node);
  virtual void visit(mitl2gta::mitl::negation_t const &node);
  virtual void visit(mitl2gta::mitl::or_t const &node);
  virtual void visit(mitl2gta::mitl::and_t const &node);
  virtual void visit(mitl2gta::mitl::untimed_next_t const &node);
  virtual void visit(mitl2gta::mitl::timed_next_t const &node);
  virtual void visit(mitl2gta::mitl::untimed_yesterday_t const &node);
  virtual void visit(mitl2gta::mitl::timed_yesterday_t const &node);
  virtual void visit(mitl2gta::mitl::untimed_finally_t const &node);
  virtual void visit(mitl2gta::mitl::timed_finally_t const &node);
  virtual void visit(mitl2gta::mitl::untimed_globally_t const &node);
  virtual void visit(mitl2gta::mitl::timed_globally_t const &node);
  virtual void visit(mitl2gta::mitl::untimed_until_t const &node);
  virtual void visit(mitl2gta::mitl::timed_until_t const &node);
  virtual void visit(mitl2gta::mitl::untimed_since_t const &node);
  virtual void visit(mitl2gta::mitl::timed_since_t const &node);

  void translate_to_transducer_system(
      mitl2gta::transducer::system_t &system,
      mitl2gta::sharer::sharer_mode_t const mode,
      mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
      mitl2gta::compilation::translation_specs_t const &specs,
      mitl2gta::compilation::node_to_truth_value_predictor_t &mapping_to_fill);

  std::map<mitl2gta::compilation::node_id_t, mitl2gta::memory::memory_id_t>
  allocate_placeholder_memory(
      mitl2gta::compilation::node_to_truth_value_predictor_t &mapping,
      mitl2gta::memory::memory_handler_t &handler) const;

  std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
  get_node(mitl2gta::compilation::node_id_t const id) const;

  void insert_new_toplevel_node(
      mitl2gta::mitl::formula_t const *parsed_node_ptr,
      std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
          compiled_node);

  void insert_new_nested_node(
      mitl2gta::mitl::formula_t const *parsed_node_ptr,
      std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
          compiled_node);

  std::vector<mitl2gta::compilation::node_id_t> topological_sort() const;

  enum translate_ctx_t {
    TRANSLATING_NESTED,
    TRANSLATING_TOPLEVEL,
  };
  translate_ctx_t _translate_ctx;

  mitl2gta::compilation::verification_mode_t const _verification_mode;

  mitl2gta::compilation::node_id_t _root_id;

  mitl2gta::mitl::subformula_placement_map_t const _subformula_to_placement_map;
  std::map<std::string, mitl2gta::compilation::node_id_t>
      _atomic_propositions_to_id;

  std::map<mitl2gta::mitl::formula_t const *, mitl2gta::compilation::node_id_t>
      _parsed_node_to_nested_id;

  std::map<mitl2gta::mitl::formula_t const *, mitl2gta::compilation::node_id_t>
      _parsed_node_to_toplevel_id;

  std::map<mitl2gta::compilation::node_id_t,
           std::shared_ptr<mitl2gta::compilation::compiled_node_t const>>
      _id_to_nested_node;

  std::map<mitl2gta::compilation::node_id_t,
           std::shared_ptr<mitl2gta::compilation::compiled_node_t const>>
      _id_to_top_level_node;

  mitl2gta::compilation::compiled_node_id_allocator_t _allocator;
};

} // namespace compilation
} // namespace mitl2gta
