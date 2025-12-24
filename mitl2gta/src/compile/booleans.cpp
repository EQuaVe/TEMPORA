/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/booleans.hpp"

namespace mitl2gta {
namespace compilation {

using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::set_node_value_t;

// compiled_negation_t
compiled_negation_t::compiled_negation_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_negation_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  if (specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR) ==
      specs.translate_for_types.end()) {
    return mitl2gta::compilation::boolean_negation_t{child()};
  }

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2}};
  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_negation_t::direct_dependencies() const {
  return {child()};
}

// compiled_or_t
compiled_or_t::compiled_or_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_or_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  if (specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR) ==
      specs.translate_for_types.end()) {
    return mitl2gta::compilation::boolean_or_t{lchild(), rchild()};
  }
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_or_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

// compiled_and_t
compiled_and_t::compiled_and_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_and_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  if (specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR) ==
      specs.translate_for_types.end()) {
    return mitl2gta::compilation::boolean_and_t{lchild(), rchild()};
  }

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_and_t::direct_dependencies() const {
  return {lchild(), rchild()};
}
} // namespace compilation
} // namespace mitl2gta
