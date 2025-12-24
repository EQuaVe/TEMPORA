/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/finally.hpp"

namespace mitl2gta {

namespace compilation {

using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::set_node_value_t;

// compiled_upper_bounded_finally_t
compiled_upper_bounded_finally_t::compiled_upper_bounded_finally_t(
    mitl2gta::interval_bound_t const upper_bound,
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child),
      mitl2gta::compilation::upper_bounded_timed_node_t(upper_bound) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_upper_bounded_finally_t::direct_dependencies() const {
  return {child()};
};

mitl2gta::compilation::truth_value_predictor_t
compiled_upper_bounded_finally_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::sharer::next_q_event_t const next_q_event =
      tools.sharer_registry.request_next_q_event(
          id(), child(), tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t q_pred_clk = next_q_event.predicting_clk;

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{q_pred_clk, interval()}});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{q_pred_clk, interval()}});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

} // namespace compilation

} // namespace mitl2gta
