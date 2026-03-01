/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */
#include <stdexcept>
#include <type_traits>
#include <variant>

#include "mitl2gta/boolean_expression/expression.hpp"
#include "mitl2gta/boolean_expression/translation.hpp"
#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/transducer/edge_discrete_attributes.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"
#include "mitl2gta/utils/string_construction.hpp"

namespace mitl2gta {

namespace transducer {

namespace {

std::string const PROVIDED_DELIM = " && ";
std::string const DO_DELIM = "; ";

int node_value_to_boolean_value(
    mitl2gta::transducer::node_value_t const value) {
  switch (value) {
  case mitl2gta::transducer::node_value_t::UNCERTAIN: {
    return static_cast<int>(mitl2gta::boolean::truth_val_t::UNCERTAIN);
  }
  case mitl2gta::transducer::node_value_t::FALSE: {
    return static_cast<int>(mitl2gta::boolean::truth_val_t::FALSE);
  }
  case mitl2gta::transducer::node_value_t::TRUE: {
    return static_cast<int>(mitl2gta::boolean::truth_val_t::TRUE);
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

std::string update_memory(std::string const &var_name, int const val) {
  return var_name + "=" + std::to_string(val);
}

std::string expr_cmp_val(std::string const &expr, int const val,
                         std::string const &cmp) {
  return expr + cmp + std::to_string(val);
}

std::string provided_for_on_cond(
    mitl2gta::transducer::transition_on_t const &on,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &mapping) {
  return std::visit(
      [&node_to_truth_value_predictor, &placeholder_memory,
       &mapping](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, mitl2gta::transducer::on_epsilon_t>) {
          return "";
        }
        if constexpr (std::is_same_v<T,
                                     mitl2gta::transducer::on_node_values_t>) {
          std::vector<std::string> conds;
          for (auto const &[id, value] : arg.id_to_value) {
            mitl2gta::boolean::expression_t const expr =
                mitl2gta::compilation::expression_for_predictor(
                    id, node_to_truth_value_predictor, placeholder_memory);

            std::string cond_expr =
                mitl2gta::boolean::boolean_expression(expr, mapping);

            int expr_val = node_value_to_boolean_value(value);

            conds.emplace_back(expr_cmp_val(cond_expr, expr_val, "=="));
          }

          return mitl2gta::join(conds.cbegin(), conds.cend(), PROVIDED_DELIM);
        }
      },
      on);
}

std::string provided_for_guards_cond(
    std::vector<mitl2gta::transducer::transition_guard_t> const &guards,
    mitl2gta::memory::memory_mapping_t const &mapping) {
  std::vector<std::string> conds;
  for (auto const &g : guards) {
    std::string cond = std::visit(
        [&mapping](auto &&arg) -> std::string {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<
                            T, mitl2gta::transducer::provided_memory_value_t>) {
            return expr_cmp_val(mapping.mapped_var(arg.mem_id), arg.value,
                                "==");
          }
        },
        g);

    conds.emplace_back(cond);
  }
  return mitl2gta::join(conds.cbegin(), conds.cend(), PROVIDED_DELIM);
}

std::string do_for_actions(
    std::vector<mitl2gta::transducer::transition_action_t> const &actions,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &mapping) {
  std::vector<std::string> dos;
  for (auto const &a : actions) {
    std::string do_string = std::visit(
        [&placeholder_memory, &mapping](auto &&arg) -> std::string {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<
                            T, mitl2gta::transducer::set_node_value_t>) {
            mitl2gta::memory::memory_id_t const mem_id =
                placeholder_memory.at(arg.node_id);
            return update_memory(mapping.mapped_var(mem_id),
                                 node_value_to_boolean_value(arg.value));
          }

          if constexpr (std::is_same_v<
                            T, mitl2gta::transducer::set_memory_value_t>) {
            return update_memory(mapping.mapped_var(arg.mem_id), arg.value);
          }
        },
        a);

    dos.emplace_back(do_string);
  }
  return mitl2gta::join(dos.cbegin(), dos.cend(), DO_DELIM);
}

} // namespace

std::string provided_attr(
    mitl2gta::transducer::edge_t const &e,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &mapping) {
  std::string provided_on = provided_for_on_cond(
      e.on(), node_to_truth_value_predictor, placeholder_memory, mapping);

  std::string provided_guards = provided_for_guards_cond(e.guards(), mapping);

  std::vector<std::string> all_conds{provided_on, provided_guards};

  all_conds = mitl2gta::filter_nonempty(all_conds);

  return mitl2gta::join(all_conds.cbegin(), all_conds.cend(), PROVIDED_DELIM);
}

std::string do_attr(
    mitl2gta::transducer::edge_t const &e,
    mitl2gta::compilation::node_to_truth_value_predictor_t const
        &node_to_truth_value_predictor,
    mitl2gta::compilation::placeholder_memory_map_t const &placeholder_memory,
    mitl2gta::memory::memory_mapping_t const &mapping) {
  return do_for_actions(e.actions(), placeholder_memory, mapping);
}

} // namespace transducer
} // namespace mitl2gta
