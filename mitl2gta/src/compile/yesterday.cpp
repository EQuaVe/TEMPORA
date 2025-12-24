/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/yesterday.hpp"

namespace mitl2gta {

namespace compilation {

using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::release_reset_clock_t;
using mitl2gta::transducer::set_node_value_t;

// compiled_untimed_yesterday_t
compiled_untimed_yesterday_t::compiled_untimed_yesterday_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child){};

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_yesterday_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t yesterday_false(tools.state_id_allocator);
  yesterday_false.make_initial();

  mitl2gta::transducer::state_t yesterday_true(tools.state_id_allocator);

  mitl2gta::transducer::edge_t const e1(
      yesterday_false.id(), yesterday_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e2(
      yesterday_false.id(), yesterday_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      yesterday_true.id(), yesterday_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e4(
      yesterday_true.id(), yesterday_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::transducer_t t{{yesterday_false, yesterday_true},
                                       {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_yesterday_t::direct_dependencies() const {
  return {child()};
}

// compiled_timed_yesterday_t
compiled_timed_yesterday_t::compiled_timed_yesterday_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child),
      mitl2gta::compilation::timed_node_t(interval) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_yesterday_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t yesterday_false(tools.state_id_allocator);

  yesterday_false.make_initial();

  mitl2gta::transducer::state_t yesterday_true(tools.state_id_allocator);

  mitl2gta::clock::clock_id_t const last_event_clk =
      tools.clk_registry.register_clock(
          mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO);

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      yesterday_false.id(), yesterday_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      yesterday_false.id(), yesterday_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      yesterday_true.id(), yesterday_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{last_event_clk, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      yesterday_true.id(), yesterday_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{last_event_clk, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      yesterday_true.id(), yesterday_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{last_event_clk, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      yesterday_true.id(), yesterday_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{last_event_clk, interval()}}));

  for (auto &e : edges) {
    e.gta_program().emplace_back(release_reset_clock_t{last_event_clk});
  }

  mitl2gta::transducer::transducer_t t{{yesterday_false, yesterday_true},
                                       std::move(edges)};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_yesterday_t::direct_dependencies() const {
  return {child()};
}
} // namespace compilation
} // namespace mitl2gta
