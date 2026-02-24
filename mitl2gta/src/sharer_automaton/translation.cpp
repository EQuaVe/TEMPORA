/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <vector>

#include "mitl2gta/sharer_automaton/sharer_types.hpp"
#include "mitl2gta/sharer_automaton/translation.hpp"
#include "mitl2gta/transducer/system.hpp"

namespace mitl2gta {

namespace sharer {

using mitl2gta::transducer::clock_val_equal_to_t;
using mitl2gta::transducer::clock_val_greater_than;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::release_reset_clock_t;
using mitl2gta::transducer::set_memory_value_t;

namespace {

mitl2gta::transducer::transducer_t time_from_first_event(
    mitl2gta::sharer::time_from_first_event_t const &time_from_first_event,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::clock::clock_id_t const time_from_first_event_clk =
      time_from_first_event.maintaining_clk;

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();
  mitl2gta::transducer::state_t s2(tools.state_id_allocator);

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s2.id(), on_node_values_t{}, {}, {},
      {release_reset_clock_t{time_from_first_event_clk}});

  mitl2gta::transducer::edge_t const e2(s2.id(), s2.id(), on_node_values_t{},
                                        {}, {}, {});

  return mitl2gta::transducer::transducer_t{{s1, s2}, {e1, e2}};
}

mitl2gta::transducer::transducer_t
next_event(mitl2gta::sharer::next_event_t const &next_event,
           mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::clock::clock_id_t const clk_id = next_event.predicting_clk;
  mitl2gta::transducer::state_t s(tools.state_id_allocator);
  s.make_initial();

  mitl2gta::transducer::edge_t e(
      s.id(), s.id(), on_node_values_t{{}}, {}, {},
      {clock_val_equal_to_t{clk_id, 0}, release_reset_clock_t{clk_id}});

  return {{s}, {e}};
}

mitl2gta::transducer::transducer_t
next_q_event(mitl2gta::sharer::next_q_event_t const &next_q_event,
             mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::clock::clock_id_t const clk_id = next_q_event.predicting_clk;
  mitl2gta::compilation::node_id_t const q_id = next_q_event.q_id;

  mitl2gta::transducer::state_t s(tools.state_id_allocator);
  s.make_initial();

  mitl2gta::transducer::edge_t e1(
      s.id(), s.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}}, {}, {},
      {clock_val_equal_to_t{clk_id, 0}, release_reset_clock_t{clk_id}});

  mitl2gta::transducer::edge_t e2(
      s.id(), s.id(), on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {},
      {});

  return {{s}, {e1, e2}};
}

mitl2gta::transducer::transducer_t p_until_q_untimed_finite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::compilation::node_id_t const p_id = p_until_q.p_id;
  mitl2gta::compilation::node_id_t const q_id = p_until_q.q_id;

  mitl2gta::memory::memory_id_t const p_until_q_val =
      p_until_q.p_until_q_truth_value;

  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      p_until_q.next_p_until_q_truth_value;

  mitl2gta::transducer::state_t s_true(tools.state_id_allocator);
  s_true.make_initial();
  mitl2gta::transducer::state_t s_false(tools.state_id_allocator);
  s_false.make_initial();
  s_false.make_final();

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_false.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_false.id(),
      on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  for (auto &e : edges) {
    if (e.from() == s_true.id()) {
      e.actions().emplace_back(
          set_memory_value_t{p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }

    if (e.to() == s_true.id()) {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }
  }

  return {{s_true, s_false}, std::move(edges)};
}

mitl2gta::transducer::transducer_t p_until_q_last_witness_finite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::compilation::node_id_t const p_id = p_until_q.p_id;
  mitl2gta::compilation::node_id_t const q_id = p_until_q.q_id;

  mitl2gta::memory::memory_id_t const p_until_q_val =
      p_until_q.p_until_q_truth_value;
  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      p_until_q.next_p_until_q_truth_value;

  mitl2gta::clock::clock_id_t const last_witness_clk =
      p_until_q.last_witness_predicting_clk.value();

  mitl2gta::transducer::state_t s_true(tools.state_id_allocator);
  s_true.make_initial();
  mitl2gta::transducer::state_t s_false(tools.state_id_allocator);
  s_false.make_initial();
  s_false.make_final();

  std::vector<mitl2gta::transducer::gta_program_t> const check_and_release{
      clock_val_equal_to_t{last_witness_clk, 0},
      release_reset_clock_t{last_witness_clk}};

  std::vector<mitl2gta::transducer::gta_program_t> const
      check_release_and_invalidate{
          clock_val_equal_to_t{last_witness_clk, 0},
          release_reset_clock_t{last_witness_clk},
          clock_val_equal_to_t{last_witness_clk, mitl2gta::EXTENDED_MINUS_INF}};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::TRUE}}},
      {}, {}, check_and_release));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_false.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, check_release_and_invalidate));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_false.id(),
      on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {release_reset_clock_t{last_witness_clk}}));

  for (auto &e : edges) {
    if (e.from() == s_true.id()) {
      e.actions().emplace_back(
          set_memory_value_t{p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }

    if (e.to() == s_true.id()) {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }
  }

  return {{s_true, s_false}, std::move(edges)};
}

mitl2gta::transducer::transducer_t p_until_q_untimed_infinite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::compilation::node_id_t const p_id = p_until_q.p_id;
  mitl2gta::compilation::node_id_t const q_id = p_until_q.q_id;

  mitl2gta::memory::memory_id_t const p_until_q_val =
      p_until_q.p_until_q_truth_value;

  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      p_until_q.next_p_until_q_truth_value;

  mitl2gta::transducer::state_t q_true(tools.state_id_allocator);
  q_true.make_initial();
  q_true.make_final();

  mitl2gta::transducer::state_t q_false_p_u_q_true(tools.state_id_allocator);
  q_false_p_u_q_true.make_initial();

  mitl2gta::transducer::state_t p_until_q_false(tools.state_id_allocator);
  p_until_q_false.make_initial();
  p_until_q_false.make_final();

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), q_true.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), q_false_p_u_q_true.id(),
      on_node_values_t{{{q_id, node_value_t::TRUE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), p_until_q_false.id(),
      on_node_values_t{{{q_id, node_value_t::TRUE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_false_p_u_q_true.id(), q_false_p_u_q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_false_p_u_q_true.id(), q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      p_until_q_false.id(), p_until_q_false.id(),
      on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      p_until_q_false.id(), q_false_p_u_q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      p_until_q_false.id(), q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  for (auto &e : edges) {
    if (e.from() == q_true.id() || e.from() == q_false_p_u_q_true.id()) {
      e.actions().emplace_back(
          set_memory_value_t{p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }

    if (e.to() == q_true.id() || e.to() == q_false_p_u_q_true.id()) {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }
  }

  return {{q_true, q_false_p_u_q_true, p_until_q_false}, std::move(edges)};
}

mitl2gta::transducer::transducer_t p_until_q_first_witness_two_state_infinite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::compilation::node_id_t const p_id = p_until_q.p_id;
  mitl2gta::compilation::node_id_t const q_id = p_until_q.q_id;

  mitl2gta::memory::memory_id_t const p_until_q_val =
      p_until_q.p_until_q_truth_value;

  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      p_until_q.next_p_until_q_truth_value;

  mitl2gta::clock::clock_id_t const first_witness_clk =
      p_until_q.first_witness_predicting_clk.value();

  mitl2gta::transducer::state_t s_true(tools.state_id_allocator);
  s_true.make_initial();
  mitl2gta::transducer::state_t s_false(tools.state_id_allocator);
  s_false.make_initial();

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_false.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_false.id(),
      on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  for (auto &e : edges) {
    if (e.from() == s_true.id()) {
      e.actions().emplace_back(
          set_memory_value_t{p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }

    if (e.to() == s_true.id()) {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});

      e.gta_program().emplace_back(clock_val_greater_than{
          first_witness_clk, mitl2gta::EXTENDED_MINUS_INF});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }
  }

  return {{s_true, s_false}, std::move(edges)};
}

mitl2gta::transducer::transducer_t p_until_q_first_witness_three_state_infinite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  return p_until_q_untimed_infinite(p_until_q, tools);
}

mitl2gta::transducer::transducer_t
p_until_q_first_last_witness_two_state_infinite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::compilation::node_id_t const p_id = p_until_q.p_id;
  mitl2gta::compilation::node_id_t const q_id = p_until_q.q_id;

  mitl2gta::memory::memory_id_t const p_until_q_val =
      p_until_q.p_until_q_truth_value;
  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      p_until_q.next_p_until_q_truth_value;

  mitl2gta::clock::clock_id_t const first_witness_clk =
      p_until_q.first_witness_predicting_clk.value();

  mitl2gta::clock::clock_id_t const last_witness_clk =
      p_until_q.last_witness_predicting_clk.value();

  mitl2gta::transducer::state_t s_true(tools.state_id_allocator);
  s_true.make_initial();
  mitl2gta::transducer::state_t s_false(tools.state_id_allocator);
  s_false.make_initial();

  std::vector<mitl2gta::transducer::gta_program_t> const check_and_release{
      clock_val_equal_to_t{last_witness_clk, 0},
      release_reset_clock_t{last_witness_clk}};

  std::vector<mitl2gta::transducer::gta_program_t> const
      check_and_release_and_invalidate{
          clock_val_equal_to_t{last_witness_clk, 0},
          release_reset_clock_t{last_witness_clk},
          clock_val_equal_to_t{last_witness_clk, mitl2gta::EXTENDED_MINUS_INF}};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::TRUE}}},
      {}, {}, check_and_release));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_true.id(), s_false.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, check_and_release_and_invalidate));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_false.id(),
      on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {release_reset_clock_t{last_witness_clk}}));

  for (auto &e : edges) {
    if (e.from() == s_true.id()) {
      e.actions().emplace_back(
          set_memory_value_t{p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }

    if (e.to() == s_true.id()) {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});

      e.gta_program().emplace_back(clock_val_greater_than{
          first_witness_clk, mitl2gta::EXTENDED_MINUS_INF});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }
  }

  return {{s_true, s_false}, std::move(edges)};
}

mitl2gta::transducer::transducer_t
p_until_q_first_last_witness_three_state_infinite(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::compilation::node_id_t const p_id = p_until_q.p_id;
  mitl2gta::compilation::node_id_t const q_id = p_until_q.q_id;

  mitl2gta::memory::memory_id_t const p_until_q_val =
      p_until_q.p_until_q_truth_value;
  mitl2gta::memory::memory_id_t const next_p_until_q_val =
      p_until_q.next_p_until_q_truth_value;

  mitl2gta::clock::clock_id_t const last_witness_clk =
      p_until_q.last_witness_predicting_clk.value();

  mitl2gta::transducer::state_t q_true(tools.state_id_allocator);
  q_true.make_initial().make_final();
  mitl2gta::transducer::state_t not_q_p_until_q_true(tools.state_id_allocator);
  not_q_p_until_q_true.make_initial();
  mitl2gta::transducer::state_t s_false(tools.state_id_allocator);
  s_false.make_initial().make_final();

  std::vector<mitl2gta::transducer::gta_program_t> const check_and_release{
      clock_val_equal_to_t{last_witness_clk, 0},
      release_reset_clock_t{last_witness_clk}};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::TRUE}}},
      {}, {}, check_and_release));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), not_q_p_until_q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::TRUE}}},
      {}, {}, check_and_release));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), not_q_p_until_q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::TRUE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      not_q_p_until_q_true.id(), not_q_p_until_q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      not_q_p_until_q_true.id(), q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::TRUE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      q_true.id(), s_false.id(), on_node_values_t{{{q_id, node_value_t::TRUE}}},
      {}, {}, check_and_release));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), s_false.id(),
      on_node_values_t{{{q_id, node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_false.id(), not_q_p_until_q_true.id(),
      on_node_values_t{
          {{p_id, node_value_t::FALSE}, {q_id, node_value_t::FALSE}}},
      {}, {}, {}));

  for (auto &e : edges) {
    if (e.from() == q_true.id() || e.from() == not_q_p_until_q_true.id()) {
      e.actions().emplace_back(
          set_memory_value_t{p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }

    if (e.to() == q_true.id() || e.to() == not_q_p_until_q_true.id()) {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_TRUE_VAL});
    }

    else {
      e.actions().emplace_back(set_memory_value_t{
          next_p_until_q_val, mitl2gta::sharer::SHARER_FALSE_VAL});
    }
  }

  return {{q_true, not_q_p_until_q_true, s_false}, std::move(edges)};
}

mitl2gta::transducer::transducer_t p_until_q(
    mitl2gta::sharer::p_until_q_sharer_t const &p_until_q,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::transducer::system_for_t const &system_for,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  switch (system_for) {
  case mitl2gta::transducer::system_for_t::FINITE_TRACE: {

    switch (p_until_q.type) {
    case mitl2gta::sharer::UNTIMED:
    case mitl2gta::sharer::TIMED_FIRST_WITNESS:
      return p_until_q_untimed_finite(p_until_q, tools);
      break;

    case mitl2gta::sharer::TIMED_FIRST_AND_LAST_WITNESS:
      return p_until_q_last_witness_finite(p_until_q, tools);
      break;
    }
    break;
  }

  case mitl2gta::transducer::system_for_t::INFINITE_TRACE: {
    switch (p_until_q.type) {
    case mitl2gta::sharer::UNTIMED: {
      return p_until_q_untimed_infinite(p_until_q, tools);
    }
    case mitl2gta::sharer::TIMED_FIRST_WITNESS: {
      switch (p_until_q_sharer_states) {
      case mitl2gta::sharer::p_until_q_sharer_states_t::TWO_STATE: {
        return p_until_q_first_witness_two_state_infinite(p_until_q, tools);
      }
      case mitl2gta::sharer::p_until_q_sharer_states_t::THREE_STATE: {
        return p_until_q_first_witness_three_state_infinite(p_until_q, tools);
      }
      }
    }
    case mitl2gta::sharer::TIMED_FIRST_AND_LAST_WITNESS: {
      switch (p_until_q_sharer_states) {
      case mitl2gta::sharer::p_until_q_sharer_states_t::TWO_STATE: {
        return p_until_q_first_last_witness_two_state_infinite(p_until_q,
                                                               tools);
      }
      case mitl2gta::sharer::p_until_q_sharer_states_t::THREE_STATE: {
        return p_until_q_first_last_witness_three_state_infinite(p_until_q,
                                                                 tools);
      }
      }
    }
    }
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

} // namespace

mitl2gta::transducer::transducer_t to_transducer(
    mitl2gta::sharer::sharer_automaton_t const &automaton,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::transducer::system_for_t const system_for,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  return std::visit(
      [&system_for, &tools, p_until_q_sharer_states](
          auto &&arg) -> mitl2gta::transducer::transducer_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<
                          T, mitl2gta::sharer::time_from_first_event_t>) {
          return time_from_first_event(arg, tools);

        } else if constexpr (std::is_same_v<T,
                                            mitl2gta::sharer::next_event_t>) {
          return next_event(arg, tools);
        } else if constexpr (std::is_same_v<T,
                                            mitl2gta::sharer::next_q_event_t>) {
          return next_q_event(arg, tools);
        } else if constexpr (std::is_same_v<
                                 T, mitl2gta::sharer::p_until_q_sharer_t>) {
          return p_until_q(arg, p_until_q_sharer_states, system_for, tools);
        } else {
          throw std::runtime_error("Incomplete visitor");
        }
      },
      automaton);
}
} // namespace sharer
} // namespace mitl2gta
