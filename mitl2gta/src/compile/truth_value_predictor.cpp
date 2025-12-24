/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/truth_value_predictor.hpp"

namespace mitl2gta {

namespace compilation {

mitl2gta::boolean::expression_t expression_for_predictor(
    mitl2gta::compilation::node_id_t const expression_for,
    mitl2gta::compilation::node_to_truth_value_predictor_t const &mapping,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::memory::memory_id_t> const &placeholder_memory) {
  return std::visit(
      [&placeholder_memory, expression_for,
       &mapping](auto &&arg) -> mitl2gta::boolean::expression_t {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<
                          T,
                          mitl2gta::compilation::predicted_by_transducer_t>) {
          return mitl2gta::boolean::variable_t{
              placeholder_memory.at(expression_for)};

        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::compilation::atomic_constant_t>) {
          return mitl2gta::boolean::constant_t{arg.val};
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::compilation::atomic_proposition_t>) {
          return mitl2gta::boolean::variable_t{placeholder_memory.at(arg.node)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::compilation::boolean_negation_t>) {
          auto child_ptr = std::make_unique<mitl2gta::boolean::expression_t>(
              expression_for_predictor(arg.node, mapping, placeholder_memory));
          return mitl2gta::boolean::negation_t{std::move(child_ptr)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::compilation::boolean_or_t>) {
          auto lchild_ptr = std::make_unique<mitl2gta::boolean::expression_t>(
              expression_for_predictor(arg.l_node, mapping,
                                       placeholder_memory));

          auto rchild_ptr = std::make_unique<mitl2gta::boolean::expression_t>(
              expression_for_predictor(arg.r_node, mapping,
                                       placeholder_memory));
          return mitl2gta::boolean::or_t{std::move(lchild_ptr),
                                         std::move(rchild_ptr)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::compilation::boolean_and_t>) {
          auto lchild_ptr = std::make_unique<mitl2gta::boolean::expression_t>(
              expression_for_predictor(arg.l_node, mapping,
                                       placeholder_memory));

          auto rchild_ptr = std::make_unique<mitl2gta::boolean::expression_t>(
              expression_for_predictor(arg.r_node, mapping,
                                       placeholder_memory));

          return mitl2gta::boolean::and_t{std::move(lchild_ptr),
                                          std::move(rchild_ptr)};
        }
      },
      mapping.at(expression_for));
}

} // namespace compilation

} // namespace mitl2gta
