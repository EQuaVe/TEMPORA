/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <variant>

#include "mitl2gta/boolean_expression/static_analysis.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"
#include "mitl2gta/transducer/static_analysis.hpp"

namespace mitl2gta {
namespace transducer {

namespace {

std::set<mitl2gta::memory::memory_id_t> active_memory_on_cond(
    mitl2gta::transducer::transition_on_t const &on,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory) {
  return std::visit(
      [&node_to_truth_value_predictor, &placeholder_memory](
          auto &&arg) -> std::set<mitl2gta::memory::memory_id_t> {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, mitl2gta::transducer::on_epsilon_t>) {
          return std::set<mitl2gta::memory::memory_id_t>();
        }
        if constexpr (std::is_same_v<T,
                                     mitl2gta::transducer::on_node_values_t>) {
          std::set<mitl2gta::memory::memory_id_t> res;
          for (auto const &[id, value] : arg.id_to_value) {
            mitl2gta::boolean::expression_t const expr =
                mitl2gta::compilation::expression_for_predictor(
                    id, node_to_truth_value_predictor, placeholder_memory);

            std::set<mitl2gta::memory::memory_id_t> const active_ids =
                mitl2gta::boolean::ids_in_expr(expr);
            res.insert(active_ids.begin(), active_ids.end());
          }
          return res;
        }
      },
      on);
}

std::set<mitl2gta::memory::memory_id_t> active_memory_guards(
    std::vector<mitl2gta::transducer::transition_guard_t> const &guards) {
  std::set<mitl2gta::memory::memory_id_t> res;

  for (auto const &g : guards) {
    std::visit(
        [&res](auto &&arg) -> void {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<
                            T, mitl2gta::transducer::provided_memory_value_t>) {
            res.insert(arg.mem_id);
          }
        },
        g);
  }

  return res;
}

std::set<mitl2gta::memory::memory_id_t> active_memory_actions(
    std::vector<mitl2gta::transducer::transition_action_t> const &actions,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory) {
  std::set<mitl2gta::memory::memory_id_t> res;
  for (auto const &a : actions) {
    std::visit(
        [&placeholder_memory, &res](auto &&arg) -> void {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<
                            T, mitl2gta::transducer::set_node_value_t>) {
            mitl2gta::memory::memory_id_t const mem_id =
                placeholder_memory.at(arg.node_id);
            res.insert(mem_id);
          }

          if constexpr (std::is_same_v<
                            T, mitl2gta::transducer::set_memory_value_t>) {
            res.insert(arg.mem_id);
          }
        },
        a);
  }
  return res;
}

std::set<mitl2gta::memory::memory_id_t> active_memory_in_edge(
    mitl2gta::transducer::edge_t const &e,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory) {
  std::set<mitl2gta::memory::memory_id_t> res;

  std::set<mitl2gta::memory::memory_id_t> const provided_active =
      active_memory_on_cond(e.on(), node_to_truth_value_predictor,
                            placeholder_memory);
  res.insert(provided_active.begin(), provided_active.end());

  std::set<mitl2gta::memory::memory_id_t> const guards_active =
      active_memory_guards(e.guards());
  res.insert(guards_active.begin(), guards_active.end());

  std::set<mitl2gta::memory::memory_id_t> const actions_active =
      active_memory_actions(e.actions(), placeholder_memory);
  res.insert(actions_active.begin(), actions_active.end());

  return res;
}
} // namespace

std::set<mitl2gta::memory::memory_id_t> active_memory(
    mitl2gta::transducer::transducer_t const &t,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory) {
  std::set<mitl2gta::memory::memory_id_t> res;
  for (auto const &e : t.edges) {
    std::set<mitl2gta::memory::memory_id_t> const active_in_e =
        active_memory_in_edge(e, node_to_truth_value_predictor,
                              placeholder_memory);
    res.insert(active_in_e.begin(), active_in_e.end());
  }

  return res;
}

bool has_epsilon_transition(
    mitl2gta::transducer::transducer_t const &transducer) {
  for (mitl2gta::transducer::edge_t const &edge : transducer.edges) {
    if (std::holds_alternative<mitl2gta::transducer::on_epsilon_t>(edge.on())) {
      return true;
    }
  }
  return false;
}
} // namespace transducer

} // namespace mitl2gta
