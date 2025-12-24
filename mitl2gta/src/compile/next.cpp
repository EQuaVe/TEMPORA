/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/next.hpp"

namespace mitl2gta {
namespace compilation {

using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::set_node_value_t;

// compiled_untimed_next_t
compiled_untimed_next_t::compiled_untimed_next_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child){};

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_next_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t next_true(tools.state_id_allocator);
  next_true.make_initial();

  mitl2gta::transducer::state_t next_false(tools.state_id_allocator);
  next_false.make_initial();

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE) {
    next_false.make_final();
  }

  mitl2gta::transducer::edge_t const e1(
      next_true.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      next_true.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      next_false.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e4(
      next_false.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{next_true, next_false},
                                       {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_next_t::direct_dependencies() const {
  return {child()};
}

// compiled_timed_next_t
compiled_timed_next_t::compiled_timed_next_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child),
      mitl2gta::compilation::timed_node_t(interval) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_next_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t next_true(tools.state_id_allocator);
  next_true.make_initial();

  mitl2gta::transducer::state_t next_false(tools.state_id_allocator);
  next_false.make_initial();

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE) {
    next_false.make_final();
  }

  mitl2gta::sharer::next_event_t next_event =
      tools.sharer_registry.request_next_event(id(), tools.clk_registry,
                                               tools.mem_handler);

  mitl2gta::transducer::edge_t const e1(
      next_true.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{next_event.predicting_clk, interval()}});

  mitl2gta::transducer::edge_t const e2(
      next_true.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{next_event.predicting_clk, interval()}});

  mitl2gta::transducer::edge_t const e3(
      next_true.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e4(
      next_false.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{next_event.predicting_clk, interval()}});

  mitl2gta::transducer::edge_t const e5(
      next_false.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{next_event.predicting_clk, interval()}});

  mitl2gta::transducer::edge_t const e6(
      next_false.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{next_true, next_false},
                                       {e1, e2, e3, e4, e5, e6}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_next_t::direct_dependencies() const {
  return {child()};
}

} // namespace compilation

} // namespace mitl2gta
