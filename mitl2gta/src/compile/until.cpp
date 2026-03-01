/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/until.hpp"
#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/interval/bound.hpp"
#include "mitl2gta/sharer_automaton/sharer_types.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"

namespace mitl2gta {

namespace compilation {

namespace {
struct general_until_prog_t {
  std::vector<mitl2gta::transducer::gta_program_t> program;
  int next_state_index;
  mitl2gta::transducer::node_value_t output;
};
} // namespace

using mitl2gta::transducer::clock_abs_val_geq_lower_bound_t;
using mitl2gta::transducer::clock_abs_val_greater_than_interval_t;
using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_less_than_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::clock_val_equal_to_t;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::provided_memory_value_t;
using mitl2gta::transducer::release_reset_clock_t;
using mitl2gta::transducer::set_node_value_t;
using mitl2gta::transducer::shift_clocks_backward_t;

using mitl2gta::transducer::provided_memory_value_t;
// compiled_untimed_until_t
compiled_untimed_until_t::compiled_untimed_until_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::UNTIMED,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

// compiled_timed_until_t
compiled_timed_until_t::compiled_timed_until_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::timed_node_t(interval),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {
  if (interval.lower_bound() == interval.upper_bound()) {
    throw std::invalid_argument("Until does not support singleton intervals");
  }
}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_until_t::generate_truth_value_predictor(
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

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q_sharer =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::
              TIMED_FIRST_AND_LAST_WITNESS,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t const x =
      p_until_q_sharer.first_witness_predicting_clk.value();
  mitl2gta::clock::clock_id_t const y =
      p_until_q_sharer.last_witness_predicting_clk.value();

  int const num_clks = 1 + (upper_bound - 1) / (upper_bound - lower_bound);

  std::vector<mitl2gta::clock::clock_id_t> x_clks;
  std::vector<mitl2gta::clock::clock_id_t> y_clks;
  for (int i = 0; i < num_clks; i++) {
    x_clks.emplace_back(tools.clk_registry.register_clock(
        mitl2gta::gta::gta_clock_type_t::PROPHECY));

    y_clks.emplace_back(tools.clk_registry.register_clock(
        mitl2gta::gta::gta_clock_type_t::PROPHECY));
  }

  std::vector<mitl2gta::transducer::state_t> locations_1;
  std::vector<mitl2gta::transducer::state_t> locations_2;

  for (int i = 0; i <= num_clks; i++) {
    locations_1.emplace_back(
        mitl2gta::transducer::state_t(tools.state_id_allocator));
    locations_2.emplace_back(
        mitl2gta::transducer::state_t(tools.state_id_allocator));
  }

  locations_1.at(0).make_initial();

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    locations_1.at(0).make_final();
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE) {
    locations_1.at(0).make_final();
  }

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{id(), node_value_t::FALSE}}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{x, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{y, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {clock_abs_val_greater_than_interval_t{x, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {clock_abs_val_less_than_interval_t{y, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(1).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::TRUE}},
      {
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          release_reset_clock_t{x_clks.at(0)},
          release_reset_clock_t{y_clks.at(0)},
          clock_abs_val_in_interval_t{x_clks.at(0), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(0), interval()},
      }));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(1).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          release_reset_clock_t{x_clks.at(0)},
          release_reset_clock_t{y_clks.at(0)},
          clock_abs_val_less_than_interval_t{x_clks.at(0), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(0), interval()},
      }));

  for (int k = 1; k < locations_1.size(); k++) {
    std::vector<general_until_prog_t> all_progs;

    std::vector<mitl2gta::transducer::gta_program_t> first_if1({
        clock_abs_val_geq_lower_bound_t{y_clks.at(k - 1), interval()},
        clock_abs_val_in_interval_t{x_clks.at(k - 1), interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{first_if1, k, node_value_t::TRUE});

    std::vector<mitl2gta::transducer::gta_program_t> first_if2({
        clock_abs_val_in_interval_t{y_clks.at(k - 1), interval()},
        clock_abs_val_less_than_interval_t{x_clks.at(k - 1), interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{first_if2, k, node_value_t::TRUE});

    std::vector<mitl2gta::transducer::gta_program_t> first_else1({
        clock_abs_val_less_than_interval_t{y_clks.at(k - 1), interval()},
        clock_abs_val_in_interval_t{y, interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{first_else1, k, node_value_t::TRUE});

    std::vector<mitl2gta::transducer::gta_program_t> first_else2({
        clock_abs_val_less_than_interval_t{y_clks.at(k - 1), interval()},
        clock_abs_val_less_than_interval_t{y, interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{first_else2, k, node_value_t::FALSE});

    if (k != locations_1.size() - 1) {
      std::vector<mitl2gta::transducer::gta_program_t> second_else1({
          clock_abs_val_less_than_interval_t{y_clks.at(k - 1), interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          release_reset_clock_t{x_clks.at(k)},
          release_reset_clock_t{y_clks.at(k)},
          clock_abs_val_in_interval_t{x_clks.at(k), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(k), interval()},
      });
      all_progs.emplace_back(
          general_until_prog_t{second_else1, k + 1, node_value_t::TRUE});

      std::vector<mitl2gta::transducer::gta_program_t> second_else2({
          clock_abs_val_less_than_interval_t{y_clks.at(k - 1), interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          release_reset_clock_t{x_clks.at(k)},
          release_reset_clock_t{y_clks.at(k)},
          clock_abs_val_less_than_interval_t{x_clks.at(k), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(k), interval()},
      });
      all_progs.emplace_back(
          general_until_prog_t{second_else2, k + 1, node_value_t::FALSE});
    }

    for (auto const &prog : all_progs) {
      std::vector<mitl2gta::transducer::gta_program_t> gta_prog = prog.program;
      int kprime = prog.next_state_index;
      node_value_t val = prog.output;

      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_1.at(k).id(), locations_1.at(kprime).id(),
          {on_node_values_t{}}, {}, {set_node_value_t{id(), val}}, gta_prog));

      gta_prog.emplace_back(clock_val_equal_to_t{x_clks.at(0), 0});
      gta_prog.emplace_back(release_reset_clock_t{x_clks.at(0)});
      gta_prog.emplace_back(
          clock_val_equal_to_t{x_clks.at(0), mitl2gta::EXTENDED_MINUS_INF});

      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_1.at(k).id(), locations_2.at(kprime).id(),
          {on_node_values_t{{{rchild(), node_value_t::TRUE}}}}, {},
          {set_node_value_t{id(), val}}, gta_prog));
    }

    for (auto const &prog : all_progs) {
      std::vector<mitl2gta::transducer::gta_program_t> gta_prog = prog.program;
      int kprime = prog.next_state_index;
      node_value_t val = prog.output;

      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_2.at(k).id(), locations_2.at(kprime).id(),
          {on_node_values_t{{{rchild(), node_value_t::FALSE}}}}, {},
          {set_node_value_t{id(), val}}, gta_prog));

      gta_prog.emplace_back(clock_val_equal_to_t{y_clks.at(0), 0});
      gta_prog.emplace_back(release_reset_clock_t{y_clks.at(0)});
      gta_prog.emplace_back(
          clock_val_equal_to_t{y_clks.at(0), mitl2gta::EXTENDED_MINUS_INF});

      if (kprime == 1) {
          edges.emplace_back(mitl2gta::transducer::edge_t(
              locations_2.at(k).id(), locations_1.at(0).id(),
              {on_node_values_t{{{rchild(), node_value_t::TRUE}}}}, {},
              {set_node_value_t{id(), val}}, gta_prog));
      } else {
        std::vector<mitl2gta::clock::clock_id_t> shiftx_clks;
        std::vector<mitl2gta::clock::clock_id_t> shifty_clks;
        for (std::size_t i = 0; i < kprime; i++) {
          shiftx_clks.emplace_back(x_clks.at(i));
          shifty_clks.emplace_back(y_clks.at(i));
        }

        gta_prog.emplace_back(shift_clocks_backward_t{shiftx_clks});
        gta_prog.emplace_back(shift_clocks_backward_t{shifty_clks});

        edges.emplace_back(mitl2gta::transducer::edge_t(
            locations_2.at(k).id(), locations_1.at(kprime - 1).id(),
            {on_node_values_t{{{rchild(), node_value_t::TRUE}}}}, {},
            {set_node_value_t{id(), val}}, gta_prog));

        gta_prog.emplace_back(clock_val_equal_to_t{x_clks.at(0), 0});
        gta_prog.emplace_back(release_reset_clock_t{x_clks.at(0)});
        gta_prog.emplace_back(
            clock_val_equal_to_t{x_clks.at(0), mitl2gta::EXTENDED_MINUS_INF});

        edges.emplace_back(mitl2gta::transducer::edge_t(
            locations_2.at(k).id(), locations_2.at(kprime - 1).id(),
            {on_node_values_t{{{rchild(), node_value_t::TRUE}}}}, {},
            {set_node_value_t{id(), val}}, gta_prog));
      }
    }
  }

  std::vector<mitl2gta::transducer::state_t> states;

  for (auto const &s : locations_1) {
    states.emplace_back(s);
  }

  for (int i = 0; i < locations_2.size(); i++) {
    if (i == 0) {
      continue;
    }
    states.emplace_back(locations_2.at(i));
  }

  mitl2gta::transducer::transducer_t t{std::move(states), std::move(edges)};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

// compiled_upper_bounded_until_t
compiled_upper_bounded_until_t::compiled_upper_bounded_until_t(
    mitl2gta::interval_bound_t const upper_bound,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::upper_bounded_timed_node_t(upper_bound),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_upper_bounded_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

mitl2gta::compilation::truth_value_predictor_t
compiled_upper_bounded_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::TIMED_FIRST_WITNESS,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t const first_witness_clk =
      p_until_q.first_witness_predicting_clk.value();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::FALSE}}},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(),
      on_node_values_t{
          {{rchild(), node_value_t::FALSE}}},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(),
      on_node_values_t{
          {{rchild(), node_value_t::FALSE}}},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

// compiled_lower_bounded_until_t
compiled_lower_bounded_until_t::compiled_lower_bounded_until_t(
    mitl2gta::interval_bound_t const lower_bound,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::lower_bounded_timed_node_t(lower_bound),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_lower_bounded_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

mitl2gta::compilation::truth_value_predictor_t
compiled_lower_bounded_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::
              TIMED_FIRST_AND_LAST_WITNESS,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t const last_witness_clk =
      p_until_q.last_witness_predicting_clk.value();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{last_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{last_witness_clk, interval()}});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}
} // namespace compilation

} // namespace mitl2gta
