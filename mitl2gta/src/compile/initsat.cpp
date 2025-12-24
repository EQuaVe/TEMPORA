/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/initsat.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"

namespace mitl2gta {

namespace compilation {

using mitl2gta::transducer::clock_abs_val_greater_than_interval_t;
using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_less_than_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::set_node_value_t;

// compiled_toplevel_node_value_t
compiled_toplevel_node_value_t::compiled_toplevel_node_value_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_toplevel_node_value_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t s_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t s_false(tools.state_id_allocator);

  mitl2gta::transducer::edge_t const e1(
      s_start.id(), s_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s_true.id(), s_true.id(), on_node_values_t{}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s_start.id(), s_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e4(
      s_false.id(), s_false.id(), on_node_values_t{}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s_start, s_false, s_true},
                                       {e1, e2, e3, e4}};
  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_toplevel_node_value_t::direct_dependencies() const {
  return {child()};
}

// compiled_toplevel_negation_t
compiled_toplevel_negation_t::compiled_toplevel_negation_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_toplevel_negation_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE &&
      specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR) ==
          specs.translate_for_types.end()) {
    return mitl2gta::compilation::boolean_negation_t{child()};
  }

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::UNCERTAIN}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(), on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3}};
  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_toplevel_negation_t::direct_dependencies() const {
  return {child()};
}

// compiled_toplevel_or_t
compiled_toplevel_or_t::compiled_toplevel_or_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_toplevel_or_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE &&
      specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR) ==
          specs.translate_for_types.end()) {
    return mitl2gta::compilation::boolean_or_t{lchild(), rchild()};
  }

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(),
      on_node_values_t{{{lchild(), node_value_t::UNCERTAIN},
                        {rchild(), node_value_t::UNCERTAIN}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(),
      on_node_values_t{{{lchild(), node_value_t::FALSE},
                        {rchild(), node_value_t::UNCERTAIN}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e5(
      s1.id(), s1.id(),
      on_node_values_t{{{lchild(), node_value_t::UNCERTAIN},
                        {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e6(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4, e5, e6}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_toplevel_or_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

// compiled_toplevel_and_t
compiled_toplevel_and_t::compiled_toplevel_and_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_toplevel_and_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE &&
      specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR) ==
          specs.translate_for_types.end()) {
    return mitl2gta::compilation::boolean_and_t{lchild(), rchild()};
  }

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(),
      on_node_values_t{{{lchild(), node_value_t::UNCERTAIN},
                        {rchild(), node_value_t::UNCERTAIN}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE},
                        {rchild(), node_value_t::UNCERTAIN}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e5(
      s1.id(), s1.id(),
      on_node_values_t{{{lchild(), node_value_t::UNCERTAIN},
                        {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::UNCERTAIN}}, {});

  mitl2gta::transducer::edge_t const e6(
      s1.id(), s1.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4, e5, e6}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_toplevel_and_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

// compiled_untimed_toplevel_next_t
compiled_untimed_toplevel_next_t::compiled_untimed_toplevel_next_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_toplevel_next_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t s_second(tools.state_id_allocator);

  mitl2gta::transducer::state_t next_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t next_false(tools.state_id_allocator);

  std::vector<mitl2gta::transducer::edge_t> edges;
  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_second.id(), on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_second.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      next_true.id(), next_true.id(), on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_second.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      next_false.id(), next_false.id(), on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == next_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == next_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == s_second.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  mitl2gta::transducer::transducer_t t{
      {s_start, s_second, next_true, next_false}, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_toplevel_next_t::direct_dependencies() const {
  return {child()};
}

// compiled_timed_toplevel_next_t
compiled_timed_toplevel_next_t::compiled_timed_toplevel_next_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child),
      mitl2gta::compilation::timed_node_t(interval) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_toplevel_next_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::sharer::time_from_first_event_t const time_from_first_event =
      tools.sharer_registry.request_time_from_first_event(
          _id, tools.clk_registry, tools.mem_handler);

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t s_second(tools.state_id_allocator);

  mitl2gta::transducer::state_t next_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t next_false(tools.state_id_allocator);

  std::vector<mitl2gta::transducer::edge_t> edges;
  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_second.id(), on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_second.id(), next_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      next_true.id(), next_true.id(), on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_second.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {},
      {clock_abs_val_not_in_interval_t{time_from_first_event.maintaining_clk,
                                       interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_second.id(), next_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      next_false.id(), next_false.id(), on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == next_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == next_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == s_second.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  mitl2gta::transducer::transducer_t t{
      {s_start, s_second, next_true, next_false}, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_toplevel_next_t::direct_dependencies() const {
  return {child()};
}

// compiled_untimed_toplevel_until_t
compiled_untimed_toplevel_until_t::compiled_untimed_toplevel_until_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_toplevel_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t p_until_q_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t p_until_q_false(tools.state_id_allocator);

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    p_until_q_false.make_final();
    p_until_q_true.make_final();
  }

  std::vector<mitl2gta::transducer::state_t> states = {s_start, p_until_q_true,
                                                       p_until_q_false};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), p_until_q_true.id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {}, {}, {}));

  edges.emplace_back(
      mitl2gta::transducer::edge_t(p_until_q_true.id(), p_until_q_true.id(),
                                   on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), p_until_q_false.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {}, {}));

  edges.emplace_back(
      mitl2gta::transducer::edge_t(p_until_q_false.id(), p_until_q_false.id(),
                                   on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == p_until_q_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == p_until_q_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == s_start.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    mitl2gta::transducer::state_t p_and_not_q(tools.state_id_allocator);
    p_and_not_q.make_initial().make_final();
    states.emplace_back(p_and_not_q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        p_and_not_q.id(), p_and_not_q.id(),
        on_node_values_t{
            {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
        {}, {set_node_value_t{_id, node_value_t::FALSE}}, {}));
  }

  mitl2gta::transducer::transducer_t t{states, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_toplevel_until_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

// compiled_timed_toplevel_until_t
compiled_timed_toplevel_until_t::compiled_timed_toplevel_until_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild),
      mitl2gta::compilation::timed_node_t(interval) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_toplevel_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::sharer::time_from_first_event_t const time_from_first_event =
      tools.sharer_registry.request_time_from_first_event(
          _id, tools.clk_registry, tools.mem_handler);

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t p_until_q_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t p_until_q_false(tools.state_id_allocator);

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    p_until_q_false.make_final();
    p_until_q_true.make_final();
  }

  std::vector<mitl2gta::transducer::state_t> states = {s_start, p_until_q_true,
                                                       p_until_q_false};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}}, {}, {},
      {clock_abs_val_less_than_interval_t{time_from_first_event.maintaining_clk,
                                          interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
      {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), p_until_q_true.id(),
      on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(
      mitl2gta::transducer::edge_t(p_until_q_true.id(), p_until_q_true.id(),
                                   on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), p_until_q_false.id(),
      on_node_values_t{{{lchild(), node_value_t::FALSE}}}, {}, {},
      {clock_abs_val_less_than_interval_t{time_from_first_event.maintaining_clk,
                                          interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), p_until_q_false.id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::FALSE}}},
      {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), p_until_q_false.id(), on_node_values_t{}, {}, {},
      {clock_abs_val_greater_than_interval_t{
          time_from_first_event.maintaining_clk, interval()}}));

  edges.emplace_back(
      mitl2gta::transducer::edge_t(p_until_q_false.id(), p_until_q_false.id(),
                                   on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == p_until_q_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == p_until_q_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == s_start.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE &&
      interval().upper_bound() == mitl2gta::INF_BOUND) {
    mitl2gta::transducer::state_t p_and_not_q(tools.state_id_allocator);
    p_and_not_q.make_initial().make_final();
    states.emplace_back(p_and_not_q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        p_and_not_q.id(), p_and_not_q.id(),
        on_node_values_t{{{lchild(), node_value_t::TRUE}}}, {},
        {set_node_value_t{_id, node_value_t::FALSE}},
        {clock_abs_val_less_than_interval_t{
            time_from_first_event.maintaining_clk, interval()}}));

    edges.emplace_back(mitl2gta::transducer::edge_t(
        p_and_not_q.id(), p_and_not_q.id(),
        on_node_values_t{
            {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::FALSE}}},
        {}, {set_node_value_t{_id, node_value_t::FALSE}},
        {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                     interval()}}));
  }

  mitl2gta::transducer::transducer_t t{states, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_toplevel_until_t::direct_dependencies() const {
  return {lchild(), rchild()};
}

// compiled_untimed_toplevel_finally_t
compiled_untimed_toplevel_finally_t::compiled_untimed_toplevel_finally_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_toplevel_finally_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t f_q_true(tools.state_id_allocator);

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    f_q_true.make_final();
  }

  std::vector<mitl2gta::transducer::state_t> states = {s_start, f_q_true};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), f_q_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      f_q_true.id(), f_q_true.id(), on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == f_q_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == s_start.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    mitl2gta::transducer::state_t not_q(tools.state_id_allocator);
    not_q.make_initial().make_final();
    states.emplace_back(not_q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        not_q.id(), not_q.id(),
        on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
        {set_node_value_t{_id, node_value_t::FALSE}}, {}));
  }

  mitl2gta::transducer::transducer_t t{states, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_toplevel_finally_t::direct_dependencies() const {
  return {child()};
}

// compiled_timed_toplevel_finally_t
compiled_timed_toplevel_finally_t::compiled_timed_toplevel_finally_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child),
      mitl2gta::compilation::timed_node_t(interval) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_toplevel_finally_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::sharer::time_from_first_event_t const time_from_first_event =
      tools.sharer_registry.request_time_from_first_event(
          _id, tools.clk_registry, tools.mem_handler);

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t f_q_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t f_q_false(tools.state_id_allocator);

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    f_q_false.make_final();
    f_q_true.make_final();
  }

  std::vector<mitl2gta::transducer::state_t> states = {s_start, f_q_true,
                                                       f_q_false};

  std::vector<mitl2gta::transducer::edge_t> edges;

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(), on_node_values_t{}, {}, {},
      {clock_abs_val_less_than_interval_t{time_from_first_event.maintaining_clk,
                                          interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), f_q_true.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      f_q_true.id(), f_q_true.id(), on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), f_q_false.id(), on_node_values_t{}, {}, {},
      {clock_abs_val_greater_than_interval_t{
          time_from_first_event.maintaining_clk, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      f_q_false.id(), f_q_false.id(), on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == f_q_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == f_q_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == s_start.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE &&
      interval().upper_bound() == mitl2gta::INF_BOUND) {
    mitl2gta::transducer::state_t not_q(tools.state_id_allocator);
    not_q.make_initial().make_final();
    states.emplace_back(not_q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        not_q.id(), not_q.id(), on_node_values_t{}, {},
        {set_node_value_t{_id, node_value_t::FALSE}},
        {clock_abs_val_less_than_interval_t{
            time_from_first_event.maintaining_clk, interval()}}));

    edges.emplace_back(mitl2gta::transducer::edge_t(
        not_q.id(), not_q.id(),
        on_node_values_t{{{child(), node_value_t::FALSE}}}, {},
        {set_node_value_t{_id, node_value_t::FALSE}},
        {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                     interval()}}));
  }

  mitl2gta::transducer::transducer_t t{states, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_toplevel_finally_t::direct_dependencies() const {
  return {child()};
}

// compiled_untimed_toplevel_globally_t
compiled_untimed_toplevel_globally_t::compiled_untimed_toplevel_globally_t(
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_toplevel_globally_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t g_q_false(tools.state_id_allocator);

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    g_q_false.make_final();
  }

  std::vector<mitl2gta::transducer::state_t> states;
  std::vector<mitl2gta::transducer::edge_t> edges;

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    mitl2gta::transducer::state_t q(tools.state_id_allocator);
    q.make_initial().make_final();
    states.emplace_back(q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        q.id(), q.id(), on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
        {set_node_value_t{_id, node_value_t::TRUE}}, {}));
  }

  states.emplace_back(s_start);
  states.emplace_back(g_q_false);

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), g_q_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      g_q_false.id(), g_q_false.id(), on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == g_q_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == s_start.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    mitl2gta::transducer::state_t q(tools.state_id_allocator);
    q.make_initial().make_final();
    states.emplace_back(q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        q.id(), q.id(), on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
        {set_node_value_t{_id, node_value_t::TRUE}}, {}));
  }

  mitl2gta::transducer::transducer_t t{states, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_toplevel_globally_t::direct_dependencies() const {
  return {child()};
}

// compiled_timed_toplevel_globally_t
compiled_timed_toplevel_globally_t::compiled_timed_toplevel_globally_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t child,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::unary_node_t(child),
      mitl2gta::compilation::timed_node_t(interval) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_toplevel_globally_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {

  mitl2gta::sharer::time_from_first_event_t const time_from_first_event =
      tools.sharer_registry.request_time_from_first_event(
          _id, tools.clk_registry, tools.mem_handler);

  mitl2gta::transducer::state_t g_q_true(tools.state_id_allocator);
  mitl2gta::transducer::state_t s_start(tools.state_id_allocator);
  s_start.make_initial();

  mitl2gta::transducer::state_t g_q_false(tools.state_id_allocator);

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    g_q_true.make_final();
    g_q_false.make_final();
  }

  std::vector<mitl2gta::transducer::state_t> states;

  std::vector<mitl2gta::transducer::edge_t> edges;

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE &&
      interval().upper_bound() == mitl2gta::INF_BOUND) {
    mitl2gta::transducer::state_t q(tools.state_id_allocator);
    q.make_initial().make_final();
    states.emplace_back(q);

    edges.emplace_back(mitl2gta::transducer::edge_t(
        q.id(), q.id(), on_node_values_t{}, {},
        {set_node_value_t{_id, node_value_t::TRUE}},
        {clock_abs_val_less_than_interval_t{
            time_from_first_event.maintaining_clk, interval()}}));

    edges.emplace_back(mitl2gta::transducer::edge_t(
        q.id(), q.id(), on_node_values_t{{{child(), node_value_t::TRUE}}}, {},
        {set_node_value_t{_id, node_value_t::TRUE}},
        {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                     interval()}}));
  }

  states.emplace_back(s_start);
  states.emplace_back(g_q_true);
  states.emplace_back(g_q_false);

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(), on_node_values_t{}, {}, {},
      {clock_abs_val_less_than_interval_t{time_from_first_event.maintaining_clk,
                                          interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), s_start.id(),
      on_node_values_t{{{child(), node_value_t::TRUE}}}, {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), g_q_false.id(),
      on_node_values_t{{{child(), node_value_t::FALSE}}}, {}, {},
      {clock_abs_val_in_interval_t{time_from_first_event.maintaining_clk,
                                   interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      g_q_false.id(), g_q_false.id(), on_node_values_t{}, {}, {}, {}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      s_start.id(), g_q_true.id(), on_node_values_t{}, {}, {},
      {clock_abs_val_greater_than_interval_t{
          time_from_first_event.maintaining_clk, interval()}}));

  edges.emplace_back(mitl2gta::transducer::edge_t(
      g_q_true.id(), g_q_true.id(), on_node_values_t{}, {}, {}, {}));

  for (auto &e : edges) {
    if (e.to() == g_q_false.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::FALSE});
    }

    else if (e.to() == g_q_true.id()) {
      e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
    }

    else if (e.to() == s_start.id()) {
      switch (specs.system_for) {
      case mitl2gta::transducer::FINITE_TRACE: {
        e.actions().emplace_back(set_node_value_t{_id, node_value_t::TRUE});
        break;
      }
      case mitl2gta::transducer::INFINITE_TRACE: {
        e.actions().emplace_back(
            set_node_value_t{_id, node_value_t::UNCERTAIN});
      }
      }
    }
  }

  mitl2gta::transducer::transducer_t t{states, edges};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_toplevel_globally_t::direct_dependencies() const {
  return {child()};
}

} // namespace compilation
} // namespace mitl2gta
