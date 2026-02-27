/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/since.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"

namespace mitl2gta {
namespace compilation {

using mitl2gta::transducer::clock_abs_val_geq_lower_bound_t;
using mitl2gta::transducer::clock_abs_val_greater_than_interval_t;
using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_less_than_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::clock_val_greater_than;
using mitl2gta::transducer::clock_val_greater_equals_t;
using mitl2gta::transducer::clock_val_less_equals_t;
using mitl2gta::transducer::clock_val_less_than_t;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_epsilon_t;
using mitl2gta::transducer::on_epsilon_node_values_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::release_reset_clock_t;
using mitl2gta::transducer::set_node_value_t;
using mitl2gta::transducer::shift_clocks_backward_t;

// compiled_untimed_since_t
compiled_untimed_since_t::compiled_untimed_since_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_since_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_since_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t psq_false(tools.state_id_allocator);
  psq_false.make_initial();
  mitl2gta::transducer::state_t psq_true(tools.state_id_allocator);

  mitl2gta::transducer::edge_t const e1(
      psq_false.id(), psq_false.id(),
      on_node_values_t{{{rchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e2(
      psq_false.id(), psq_true.id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e3(
      psq_true.id(), psq_false.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e4(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e5(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e6(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::transducer_t t{{psq_true, psq_false},
                                       {e1, e2, e3, e4, e5, e6}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

// compiled_timed_since_t
compiled_timed_since_t::compiled_timed_since_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::timed_node_t(interval),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_since_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::boundval_t const lower_bound = interval().lower_bound().val;
  mitl2gta::boundval_t const upper_bound = interval().upper_bound().val;

  if (upper_bound == mitl2gta::INF_VAL) {
    throw std::runtime_error("Upper bound should be finite");
  }

  if (upper_bound <= lower_bound) {
    throw std::runtime_error("Invalid interval");
  }

  int const num_clocks = 1 + (upper_bound - 1) / (upper_bound - lower_bound);

  std::vector<mitl2gta::clock::clock_id_t> x_clks;
  std::vector<mitl2gta::clock::clock_id_t> y_clks;
  for (int i = 0; i < num_clocks; i++) {
    x_clks.emplace_back(tools.clk_registry.register_clock(
        mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO));

    y_clks.emplace_back(tools.clk_registry.register_clock(
        mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO));
  }

  int const num_locs = 1 + num_clocks;

  std::vector<mitl2gta::transducer::state_t> locations;
  for (int i = 0; i < num_locs; i++) {
    locations.emplace_back(
        mitl2gta::transducer::state_t(tools.state_id_allocator));
  }

  locations.at(0).make_initial();

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations.at(0).id(), locations.at(0).id(),
      on_node_values_t{{{rchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{id(), node_value_t::FALSE}}, {}));

  for (int i = 1; i < num_locs; i++) {
    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(0).id(),
        on_node_values_t{
            {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
        {}, {set_node_value_t{id(), node_value_t::FALSE}}, {}));
  }

  mitl2gta::clock::clock_id_t const x1 = x_clks.at(0);
  mitl2gta::clock::clock_id_t const y1 = y_clks.at(0);

  for (int i = 1; i < num_locs; i++) {
    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(1).id(),
        on_node_values_t{
            {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::TRUE}}},
        {}, {set_node_value_t{id(), node_value_t::FALSE}},
        {{release_reset_clock_t{x1}, {release_reset_clock_t{y1}}}}));
  }

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations.at(0).id(), locations.at(1).id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {{release_reset_clock_t{x1}, {release_reset_clock_t{y1}}}}));

  std::vector<std::vector<mitl2gta::transducer::gta_program_t>> const
      either_in_interval = {
          {clock_abs_val_in_interval_t{x1, interval()},
           clock_abs_val_less_than_interval_t{y1, interval()}},

          {clock_abs_val_in_interval_t{x1, interval()},
           {clock_abs_val_in_interval_t{y1, interval()}}},

          {clock_abs_val_greater_than_interval_t{x1, interval()},
           clock_abs_val_in_interval_t{y1, interval()}}};

  std::vector<std::vector<mitl2gta::transducer::gta_program_t>> const
      both_out_intervals = {
          {clock_abs_val_less_than_interval_t{x1, interval()},
           clock_abs_val_less_than_interval_t{y1, interval()}},

          {clock_abs_val_less_than_interval_t{x1, interval()},
           {clock_abs_val_greater_than_interval_t{y1, interval()}}},

          {clock_abs_val_greater_than_interval_t{x1, interval()},
           {clock_abs_val_less_than_interval_t{y1, interval()}}},

          {clock_abs_val_greater_than_interval_t{x1, interval()},
           {clock_abs_val_greater_than_interval_t{y1, interval()}}}};

  for (int i = 1; i < num_locs; i++) {
    for (auto const &gta_prog : either_in_interval) {
    std::vector<mitl2gta::transducer::gta_program_t> gta_prog_copy = gta_prog;

    // NEW: only allow this transition while shift condition is not enabled
    gta_prog_copy.emplace_back(clock_val_less_than_t{x_clks.at(i - 1), lower_bound});

    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(i).id(),
        on_node_values_t{{{lchild(), node_value_t::TRUE},
                          {rchild(), node_value_t::FALSE}}},
        {}, {set_node_value_t{id(), node_value_t::TRUE}}, gta_prog_copy));
  }

  for (auto const &gta_prog : both_out_intervals) {
    std::vector<mitl2gta::transducer::gta_program_t> gta_prog_copy = gta_prog;

    // NEW: only allow this transition while shift condition is not enabled
    gta_prog_copy.emplace_back(clock_val_less_than_t{x_clks.at(i - 1), lower_bound});

    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(i).id(),
        on_node_values_t{{{lchild(), node_value_t::TRUE},
                          {rchild(), node_value_t::FALSE}}},
        {}, {set_node_value_t{id(), node_value_t::FALSE}}, gta_prog_copy));
  }
}
   
  for (int i = 1; i < num_locs; i++) {
  for (auto const &gta_prog : either_in_interval) {
    std::vector<mitl2gta::transducer::gta_program_t> gta_prog_copy = gta_prog;

    // NEW: block this transition once the shift condition becomes enabled at loc[i]
    gta_prog_copy.emplace_back(clock_val_less_than_t{x_clks.at(i-1), lower_bound});

    gta_prog_copy.emplace_back(
        clock_val_less_than_t{x_clks.at(i - 1), upper_bound - lower_bound});
    gta_prog_copy.emplace_back(release_reset_clock_t{y_clks.at(i - 1)});

    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(i).id(),
        on_node_values_t{
            {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
        {}, {set_node_value_t{id(), node_value_t::TRUE}}, gta_prog_copy));
  }

  for (auto const &gta_prog : both_out_intervals) {
    std::vector<mitl2gta::transducer::gta_program_t> gta_prog_copy = gta_prog;

    // NEW: block this transition once the shift condition becomes enabled at loc[i]
    gta_prog_copy.emplace_back(clock_val_less_than_t{x_clks.at(i-1), lower_bound});

    gta_prog_copy.emplace_back(
        clock_val_less_than_t{x_clks.at(i - 1), upper_bound - lower_bound});
    gta_prog_copy.emplace_back(release_reset_clock_t{y_clks.at(i - 1)});

    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(i).id(),
        on_node_values_t{
            {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
        {}, {set_node_value_t{id(), node_value_t::FALSE}}, gta_prog_copy));
  }
}

  for (int i = 1; i < num_locs - 1; i++) {
    for (auto const &gta_prog : either_in_interval) {
      std::vector<mitl2gta::transducer::gta_program_t> gta_prog_copy = gta_prog;
      gta_prog_copy.emplace_back(
          clock_val_greater_equals_t{x_clks.at(i - 1), upper_bound - lower_bound});
      gta_prog_copy.emplace_back(release_reset_clock_t{x_clks.at(i)});
      gta_prog_copy.emplace_back(release_reset_clock_t{y_clks.at(i)});

      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations.at(i).id(), locations.at(i + 1).id(),
          on_node_values_t{
              {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
          {}, {set_node_value_t{id(), node_value_t::TRUE}}, gta_prog_copy));
    }

    for (auto const &gta_prog : both_out_intervals) {
      std::vector<mitl2gta::transducer::gta_program_t> gta_prog_copy = gta_prog;
      gta_prog_copy.emplace_back(
          clock_val_greater_equals_t{x_clks.at(i - 1), upper_bound - lower_bound});
      gta_prog_copy.emplace_back(release_reset_clock_t{x_clks.at(i)});
      gta_prog_copy.emplace_back(release_reset_clock_t{y_clks.at(i)});

      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations.at(i).id(), locations.at(i + 1).id(),
          on_node_values_t{
              {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
          {}, {set_node_value_t{id(), node_value_t::FALSE}}, gta_prog_copy));
    }
  }

  for (int i = 2; i < num_locs; i++) {
    std::vector<mitl2gta::clock::clock_id_t> shift_x_clks;
    std::vector<mitl2gta::clock::clock_id_t> shift_y_clks;
    for (int clk_idx = 0; clk_idx < i; clk_idx++) {
      shift_x_clks.emplace_back(x_clks.at(clk_idx));
      shift_y_clks.emplace_back(y_clks.at(clk_idx));
    }

    mitl2gta::clock::clock_id_t const x2 = x_clks.at(1);
    edges.emplace_back(mitl2gta::transducer::edge_t(
        locations.at(i).id(), locations.at(i - 1).id(),
        on_epsilon_node_values_t{{{lchild(), node_value_t::TRUE}}}, {}, {},
        {{clock_abs_val_geq_lower_bound_t{x2, interval()},
          {shift_clocks_backward_t{shift_x_clks}},
          {shift_clocks_backward_t{shift_y_clks}}}}));
  }

  mitl2gta::transducer::transducer_t t{std::move(locations), std::move(edges)};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_since_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

// compiled_upper_bounded_since_t
compiled_upper_bounded_since_t::compiled_upper_bounded_since_t(
    mitl2gta::interval_bound_t const upper_bound,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::upper_bounded_timed_node_t(upper_bound),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_upper_bounded_since_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

mitl2gta::compilation::truth_value_predictor_t
compiled_upper_bounded_since_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t psq_false(tools.state_id_allocator);
  psq_false.make_initial();
  mitl2gta::transducer::state_t psq_true(tools.state_id_allocator);

  mitl2gta::clock::clock_id_t const last_witness_clk =
      tools.clk_registry.register_clock(
          mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO);

  mitl2gta::transducer::edge_t const e1(
      psq_false.id(), psq_false.id(),
      on_node_values_t{{{rchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {release_reset_clock_t{last_witness_clk}});

  mitl2gta::transducer::edge_t const e2(
      psq_false.id(), psq_true.id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {release_reset_clock_t{last_witness_clk}});

  mitl2gta::transducer::edge_t const e3(
      psq_true.id(), psq_false.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}},
      {release_reset_clock_t{last_witness_clk}});

  mitl2gta::transducer::edge_t const e4(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{last_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e5(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{last_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e6(
      psq_true.id(), psq_true.id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {release_reset_clock_t{last_witness_clk}});

  mitl2gta::transducer::transducer_t t{{psq_true, psq_false},
                                       {e1, e2, e3, e4, e5, e6}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

// compiled_lower_bounded_since_t
compiled_lower_bounded_since_t::compiled_lower_bounded_since_t(
    mitl2gta::interval_bound_t const lower_bound,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::lower_bounded_timed_node_t(lower_bound),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_lower_bounded_since_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

mitl2gta::compilation::truth_value_predictor_t
compiled_lower_bounded_since_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t psq_false(tools.state_id_allocator);
  psq_false.make_initial();
  mitl2gta::transducer::state_t psq_true(tools.state_id_allocator);

  mitl2gta::clock::clock_id_t const first_witness_clk =
      tools.clk_registry.register_clock(
          mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO);

  mitl2gta::transducer::edge_t const e1(
      psq_false.id(), psq_false.id(),
      on_node_values_t{{{rchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {release_reset_clock_t{first_witness_clk}});

  mitl2gta::transducer::edge_t const e2(
      psq_false.id(), psq_true.id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {release_reset_clock_t{first_witness_clk}});

  mitl2gta::transducer::edge_t const e3(
      psq_true.id(), psq_false.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}},
      {release_reset_clock_t{first_witness_clk}});

  mitl2gta::transducer::edge_t const e4(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e5(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e6(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e7(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e8(
      psq_true.id(), psq_true.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}},
      {release_reset_clock_t{first_witness_clk}});

  mitl2gta::transducer::transducer_t t{{psq_true, psq_false},
                                       {e1, e2, e3, e4, e5, e6, e7, e8}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

} // namespace compilation
} // namespace mitl2gta
