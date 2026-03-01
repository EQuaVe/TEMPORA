/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <deque>
#include <variant>
#include <vector>

#include "mitl2gta/gta_system/synchronization.hpp"
#include "mitl2gta/transducer/edge_discrete_attributes.hpp"
#include "mitl2gta/transducer/gta_program.hpp"
#include "mitl2gta/transducer/static_analysis.hpp"
#include "mitl2gta/transducer/translation.hpp"
#include "mitl2gta/utils/string_construction.hpp"

#include "mitl2gta/gta_system/edge.hpp"
#include "mitl2gta/gta_system/location.hpp"
#include "mitl2gta/memory/memory_mapping.hpp"
#include "mitl2gta/transducer/state.hpp"

namespace mitl2gta {

namespace transducer {

namespace {

std::string const INITIAL_LOC_ATTR = "initial";
std::string const URGENT_LOC_ATTR = "urgent";
std::string const CONTROL_MODEL_STATE = "control_model";

std::string proc_name(std::size_t const transducer_idx) {
  return "process" + std::to_string(transducer_idx);
}

std::string control_state_for_transducer(std::size_t const transducer_idx) {
  return "control" + std::to_string(transducer_idx);
}

std::string event_name(mitl2gta::transducer::transition_on_t const &on,
                       std::size_t const transducer_idx) {

  return std::visit(
      [transducer_idx](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T,
                                     mitl2gta::transducer::on_node_values_t>) {
          return "consume_event" + std::to_string(transducer_idx);
        }

        else if constexpr (std::is_same_v<T,
                                          mitl2gta::transducer::on_epsilon_t>) {
          return "epsilon_event" + std::to_string(transducer_idx);
        }

        else {
          throw std::runtime_error("Incomplete visitor");
        }
      },
      on);
}

} // namespace

synchronized_gta_system_t::synchronized_gta_system_t(
    mitl2gta::transducer::system_t &system,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::compilation::truth_value_predictor_t> const
        &node_to_truth_value_predictor,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::memory::memory_id_t> const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &memory_mapping,
    mitl2gta::compilation::verification_mode_t const verification_mode)
    : _system(system),
      _node_to_truth_value_predictor(node_to_truth_value_predictor),
      _placeholder_memory(placeholder_memory), _memory_mapping(memory_mapping),
      _verification_mode(verification_mode) {

  std::deque<mitl2gta::transducer::transducer_t> const transducers =
      _system.transducers();

  if (transducers.empty()) {
    throw std::invalid_argument(
        "Cannot synchronize empty system of transducers");
  }

  for (transducer_idx_t i = 0; i < transducers.size(); i++) {
    _procs.emplace_back(to_gta_proc(transducers.at(i), i));
  }

  add_controller_with_sync();
}

void synchronized_gta_system_t::add_controller_with_sync() {
  std::deque<mitl2gta::transducer::transducer_t> const transducers =
      _system.transducers();

  std::size_t const num_transducers = transducers.size();

  std::vector<mitl2gta::gta::location_t> locs;
  std::vector<mitl2gta::gta::edge_t> edges;
  std::string const name = "controller";

  std::string const init_loc = "control_init";
  mitl2gta::gta::attributes_t init_attributes{{INITIAL_LOC_ATTR, ""}};
  if (_system.system_for() ==
      mitl2gta::transducer::system_for_t::FINITE_TRACE) {
    std::string final_label = "control_final";
    init_attributes.insert({{"labels", final_label}});
    _labels.insert(final_label);
  }
  locs.emplace_back(mitl2gta::gta::location_t{init_loc, init_attributes});

  if (_verification_mode ==
      mitl2gta::compilation::verification_mode_t::MODEL_CHECKING) {
    mitl2gta::gta::attributes_t const model_state_attributes{
        {URGENT_LOC_ATTR, ""}};
    locs.emplace_back(
        mitl2gta::gta::location_t{CONTROL_MODEL_STATE, model_state_attributes});
  }

  for (transducer_idx_t i = 0; i < num_transducers; i++) {
    std::string const loc = control_state_for_transducer(i);
    mitl2gta::gta::attributes_t const attributes{{URGENT_LOC_ATTR, ""}};
    locs.emplace_back(mitl2gta::gta::location_t{loc, attributes});
  }

  std::string const start_event = "start_round";
  _events.insert(start_event);

  switch (_system.system_for()) {
  case mitl2gta::transducer::system_for_t::INFINITE_TRACE: {
    mitl2gta::clock::clock_id_t const zeno_clk =
        _system.clock_registry().register_clock(
            mitl2gta::gta::gta_clock_type_t::PROPHECY);
    std::string const clk_name = mitl2gta::clock::clock_name(zeno_clk);

    std::vector<mitl2gta::transducer::gta_program_t> clk_less_minus_inf_prog{
        clock_val_greater_than{zeno_clk, mitl2gta::EXTENDED_MINUS_INF}};
    std::vector<std::string> gta_programs =
        mitl2gta::transducer::gta_programs_disjunction(
            clk_less_minus_inf_prog, _system.clock_registry());

    for (std::string const &g : gta_programs) {
      mitl2gta::gta::attributes_t attributes{{"gta_program", g}};
      edges.emplace_back(mitl2gta::gta::edge_t{init_loc, locs.at(1).name,
                                               start_event, attributes});
    }

    std::vector<mitl2gta::transducer::gta_program_t> zeno_gta_prog{
        clock_val_equal_to_t{zeno_clk, 0}, release_reset_clock_t{zeno_clk},
        clock_val_less_equals_t{zeno_clk, -1},
        clock_val_greater_than{zeno_clk, mitl2gta::EXTENDED_MINUS_INF}};
    gta_programs = mitl2gta::transducer::gta_programs_disjunction(
        zeno_gta_prog, _system.clock_registry());

    for (std::string const &g : gta_programs) {
      mitl2gta::gta::attributes_t attributes{{"gta_program", g}};
      edges.emplace_back(mitl2gta::gta::edge_t{init_loc, locs.at(1).name,
                                               start_event, attributes});
    }

    break;
  }

  case mitl2gta::transducer::system_for_t::FINITE_TRACE: {
    mitl2gta::gta::edge_t e{init_loc, locs.at(1).name, start_event, {}};
    edges.emplace_back(e);
    break;
  }

  default: {
    throw std::runtime_error("Incomplete switch statement");
  }
  }

  transducer_idx_t const first_transducer_index =
      _verification_mode == mitl2gta::compilation::MODEL_CHECKING ? 2 : 1;

  if (_verification_mode == mitl2gta::compilation::MODEL_CHECKING) {
    std::string const model_move_event = "move_model";
    _events.insert(model_move_event);
    mitl2gta::gta::edge_t e{CONTROL_MODEL_STATE,
                            locs.at(first_transducer_index).name,
                            model_move_event};
    edges.emplace_back(e);
  }

  for (transducer_idx_t i = 0; i < num_transducers; i++) {
    std::string const from =
        locs.at((i + first_transducer_index) % (locs.size())).name;
    std::string const to =
        locs.at((i + 1 + first_transducer_index) % (locs.size())).name;

    std::string const move_control_event = "move" + std::to_string(i);
    _events.insert(move_control_event);

    mitl2gta::gta::edge_t e{from, to, move_control_event};
    edges.emplace_back(e);
    mitl2gta::gta::atomic_sync_constraint_t const control_sync{
        name, move_control_event};
    mitl2gta::gta::atomic_sync_constraint_t const transducer_sync{
        proc_name(i), event_name(mitl2gta::transducer::on_node_values_t{}, i)};

    mitl2gta::gta::sync_constraints sync{control_sync, transducer_sync};
    _sync_constraints.emplace_back(sync);

    if (mitl2gta::transducer::has_epsilon_transition(transducers.at(i))) {
      std::string const stay_control_event = "stay" + std::to_string(i);
      _events.insert(stay_control_event);

      mitl2gta::gta::edge_t e{from, from, stay_control_event};
      edges.emplace_back(e);
      mitl2gta::gta::atomic_sync_constraint_t const control_sync{
          name, stay_control_event};
      mitl2gta::gta::atomic_sync_constraint_t const transducer_sync{
          proc_name(i), event_name(mitl2gta::transducer::on_epsilon_t{}, i)};

      mitl2gta::gta::sync_constraints sync{control_sync, transducer_sync};
      _sync_constraints.emplace_back(sync);
    }
  }

  _procs.emplace_back(mitl2gta::gta::process_t{name, locs, edges});
}

mitl2gta::gta::location_t synchronized_gta_system_t::to_gta_loc(
    mitl2gta::transducer::state_t const &state, std::string const &label) {
  std::string name = mitl2gta::transducer::name_from_id(state.id());
  mitl2gta::gta::attributes_t attributes;

  if (state.is_initial()) {
    attributes.insert({INITIAL_LOC_ATTR, ""});
  }

  if (state.is_final()) {
    attributes.insert({"labels", label});
    _labels.insert(label);
  }

  if (state.is_urgent()) {
    attributes.insert({URGENT_LOC_ATTR, ""});
  }

  return mitl2gta::gta::location_t{name, attributes};
}

std::vector<mitl2gta::gta::edge_t>
synchronized_gta_system_t::to_gta_edge(mitl2gta::transducer::edge_t const &edge,
                                       transducer_idx_t const idx) {

  std::vector<mitl2gta::gta::edge_t> res;
  mitl2gta::gta::attributes_t attrs;

  std::string provided_string =
      mitl2gta::transducer::provided_attr(edge, _node_to_truth_value_predictor,
                                          _placeholder_memory, _memory_mapping);

  if (!provided_string.empty()) {
    attrs.insert({"provided", provided_string});
  }

  std::string do_string =
      mitl2gta::transducer::do_attr(edge, _node_to_truth_value_predictor,
                                    _placeholder_memory, _memory_mapping);

  if (!do_string.empty()) {
    attrs.insert({"do", do_string});
  }

  std::string const from_loc_name =
      mitl2gta::transducer::name_from_id(edge.from());
  std::string const to_loc_name = mitl2gta::transducer::name_from_id(edge.to());

  std::string const event = event_name(edge.on(), idx);
  _events.insert(event);

  for (std::string const &g : mitl2gta::transducer::gta_programs_disjunction(
           edge.gta_program(), _system.clock_registry())) {
    mitl2gta::gta::attributes_t attrs_copy = attrs;
    if (!g.empty()) {
      attrs_copy["gta_program"] = g;
    }
    res.emplace_back(
        mitl2gta::gta::edge_t{from_loc_name, to_loc_name, event, attrs_copy});
  }

  return res;
}

mitl2gta::gta::process_t synchronized_gta_system_t::to_gta_proc(
    mitl2gta::transducer::transducer_t const &transducer,
    transducer_idx_t const idx) {
  std::vector<mitl2gta::gta::location_t> locations;

  std::size_t next_label_idx = _labels.size();
  std::string state_label = "final" + std::to_string(next_label_idx);
  for (auto const &s : transducer.states) {
    locations.emplace_back(to_gta_loc(s, state_label));
  }

  std::string const name = proc_name(idx);

  std::vector<mitl2gta::gta::edge_t> edges;

  for (auto const &e : transducer.edges) {
    std::vector<mitl2gta::gta::edge_t> translated_edges = to_gta_edge(e, idx);

    for (auto const &e : translated_edges) {
      edges.emplace_back(e);
    }
  }

  return mitl2gta::gta::process_t{name, locations, edges};
}

mitl2gta::gta::system_t synchronized_gta_system_t::gta_system() const {

  std::vector<std::string> events(_events.begin(), _events.end());
  std::vector<std::string> labels(_labels.begin(), _labels.end());

  std::vector<mitl2gta::gta::array_integer_variables_t> int_variables =
      _memory_mapping.int_vars();

  std::vector<mitl2gta::gta::gta_clock_variable_t> clock_variables =
      mitl2gta::clock::clock_vars_from_reg(_system.clock_registry());

  return mitl2gta::gta::system_t{
      "mitl2gta_translation", events, labels,           int_variables,
      clock_variables,        _procs, _sync_constraints};
}
} // namespace transducer

} // namespace mitl2gta
