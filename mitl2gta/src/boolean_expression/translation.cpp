/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <stdexcept>
#include <variant>

#include "mitl2gta/boolean_expression/translation.hpp"

namespace mitl2gta {
namespace boolean {

using mitl2gta::boolean::truth_val_t::FALSE;
using mitl2gta::boolean::truth_val_t::TRUE;

std::string
boolean_expression(mitl2gta::boolean::expression_t const &expr,
                   mitl2gta::memory::memory_mapping_t const &mapping) {

  // Negation
  static_assert(1 - TRUE == FALSE);

  // AND
  static_assert(FALSE * FALSE == FALSE);
  static_assert(FALSE * TRUE == FALSE);
  static_assert(TRUE * TRUE == TRUE);

  // OR
  static_assert(1 - (1 - TRUE) * (1 - FALSE) == TRUE);
  static_assert(1 - (1 - TRUE) * (1 - TRUE) == TRUE);
  static_assert(1 - (1 - FALSE) * (1 - FALSE) == FALSE);

  return std::visit(
      [&mapping](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, mitl2gta::boolean::constant_t>) {
          switch (arg.val) {
          case mitl2gta::mitl::constant_value_t::BOOL_TRUE: {
            return std::to_string(TRUE);
          }
          case mitl2gta::mitl::constant_value_t::BOOL_FALSE: {
            return std::to_string(FALSE);
          }
          default: {
            throw std::runtime_error("Incomplete switch statement");
          }
          }
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::variable_t>) {
          return mapping.mapped_var(arg.mem_id);
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::negation_t>) {
          std::string child_expr =
              mitl2gta::boolean::boolean_expression(*(arg.child), mapping);
          return "(1-" + child_expr + ")";
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::or_t>) {
          std::string lchild_expr =
              mitl2gta::boolean::boolean_expression(*(arg.lchild), mapping);

          std::string rchild_expr =
              mitl2gta::boolean::boolean_expression(*(arg.rchild), mapping);

          return "(1-(1-" + lchild_expr + ")*(1-" + rchild_expr + "))";
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::and_t>) {
          std::string lchild_expr =
              mitl2gta::boolean::boolean_expression(*(arg.lchild), mapping);

          std::string rchild_expr =
              mitl2gta::boolean::boolean_expression(*(arg.rchild), mapping);

          return "(" + lchild_expr + "*" + rchild_expr + ")";
        }
      },
      expr);
}
} // namespace boolean
} // namespace mitl2gta
