/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/atomic_proposition.hpp"

namespace mitl2gta {

namespace compilation {

compiled_atomic_proposition_t::compiled_atomic_proposition_t(
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_atomic_proposition_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  if (specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::ATOMIC_PROPOSITION) ==
      specs.translate_for_types.end()) {
    return mitl2gta::compilation::atomic_proposition_t{_id};
  }

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), mitl2gta::transducer::on_node_values_t(), {},
      {mitl2gta::transducer::set_node_value_t{
          _id, mitl2gta::transducer::node_value_t::TRUE}},
      {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), mitl2gta::transducer::on_node_values_t(), {},
      {mitl2gta::transducer::set_node_value_t{
          _id, mitl2gta::transducer::node_value_t::FALSE}},
      {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_atomic_proposition_t::direct_dependencies() const {
  return {};
}

} // namespace compilation

} // namespace mitl2gta
